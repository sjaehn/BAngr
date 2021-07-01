/* B.Angr
 * Dynamic distorted bandpass filter plugin
 *
 * Copyright (C) 2021 by Sven Jähnichen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "BAngr.hpp"

#include <cstdio>
#include <string>
#include <stdexcept>
#include "ControllerLimits.hpp"

#define LIMIT(g , min, max) ((g) > (max) ? (max) : ((g) < (min) ? (min) : (g)))
#define CO2DB(x) (20.0f * log10f ((x)))

const float flexTime[NR_FLEX] = {1.0f, 0.05f};

BAngr::BAngr (double samplerate, const LV2_Feature* const* features) :
	map(NULL),
	rate (samplerate),
	xcursor (0.5f),
	ycursor (0.5f),
	listen (false),
	rnd (time (0)), 
	bidist (-1.0, 1.0),
	count (0),
	fader (0.0f),
	speed (0.0f),
	speedrand (0.0f),
	dspeedrand (0.0f),
	speedflex (0.0f),
	spin (0.0f),
	spinrand (0.0f),
	dspinrand (0.0f),
	spinflex (0.0f),
	spindir (1.0f),
	ang (2.0 * M_PI * bidist (rnd)),
	speedlevel (0.0f),
	spinlevel (0.0f),
	lowpassFilter (rate, 200.0, 8),
	highpassFilter (rate, 4000.0, 8),
	bandpassFilter (rate, 200.0, 4000.0, 8),
	controlPort (nullptr),
	notifyPort (nullptr),
	audioInput1 (nullptr), 
	audioInput2 (nullptr),
	audioOutput1 (nullptr), 
	audioOutput2 (nullptr),
	newControllers {nullptr},
	controllers {0.0f},
	ui_on (false),
	xregion (samplerate)

{
	//Scan host features for URID map
	LV2_URID_Map* m = NULL;
	for (int i = 0; features[i]; ++i)
	{
		if (strcmp(features[i]->URI, LV2_URID__map) == 0)
		{
			m = (LV2_URID_Map*) features[i]->data;
		}
	}
	if (!m) throw std::invalid_argument ("Host does not support urid:map");

	//Map URIS
	map = m;
	getURIs (m, &urids);

	// Initialize forge
	lv2_atom_forge_init (&forge, map);

}

BAngr::~BAngr () {}

void BAngr::connect_port(uint32_t port, void *data)
{
	switch (port) {
	case CONTROL:
		controlPort = (LV2_Atom_Sequence*) data;
		break;
	case NOTIFY:
		notifyPort = (LV2_Atom_Sequence*) data;
		break;
	case AUDIO_IN_1:
		audioInput1 = (float*) data;
		break;
	case AUDIO_IN_2:
		audioInput2 = (float*) data;
		break;
	case AUDIO_OUT_1:
		audioOutput1 = (float*) data;
		break;
	case AUDIO_OUT_2:
		audioOutput2 = (float*) data;
		break;
	default:
		if ((port >= CONTROLLERS) && (port < CONTROLLERS + NR_CONTROLLERS)) newControllers[port - CONTROLLERS] = (float*) data;
	}
}

void BAngr::run (uint32_t n_samples)
{
	// Check ports
	if ((!controlPort) || (!notifyPort) || (!audioInput1) || (!audioInput2) || (!audioOutput1) || (!audioOutput2)) return;
	for (int i = 0; i < NR_CONTROLLERS; ++i)
	{
		if (!newControllers[i]) return;
	}

	// Update controller values
	for (int i = 0; i < NR_CONTROLLERS; ++i)
	{
		if (controllers[i] != *newControllers[i]) controllers[i] = controllerLimits[i].validate (*newControllers[i]);
	}

	// Prepare forge buffer and initialize atom sequence
	const uint32_t space = notifyPort->atom.size;
	lv2_atom_forge_set_buffer(&forge, (uint8_t*) notifyPort, space);
	lv2_atom_forge_sequence_head(&forge, &frame, 0);

	// Process GUI data
	uint32_t last_t =0;
	LV2_ATOM_SEQUENCE_FOREACH(controlPort, ev)
	{
		if (lv2_atom_forge_is_object_type(&forge, ev->body.type))
		{
			const LV2_Atom_Object* obj = (const LV2_Atom_Object*)&ev->body;

			// UI on
			if (obj->body.otype == urids.bangr_uiOn) 
			{
				ui_on = true;
				listen = false;
			}

			// UI off
			else if (obj->body.otype == urids.bangr_uiOff) 
			{
				ui_on = false;
				listen = false;
			}

			// Cursor
			else if (obj->body.otype == urids.bangr_cursorEvent)
			{
				LV2_Atom *ox = NULL, *oy = NULL;
				lv2_atom_object_get (obj,
						     urids.bangr_xcursor, &ox,
						     urids.bangr_ycursor, &oy,
						     NULL);

				if (ox && (ox->type == urids.atom_Float) && oy && (oy->type == urids.atom_Float))
				{
					xcursor = ((LV2_Atom_Float*)ox)->body;
					ycursor = ((LV2_Atom_Float*)oy)->body;
					listen = true;
				}

				else listen = false;
			}

			// Play samples
			uint32_t next_t = (ev->time.frames < n_samples ? ev->time.frames : n_samples);
			play (last_t, next_t);
			last_t = next_t;
		}
	}

	// Play remaining samples
	if (last_t < n_samples) play (last_t, n_samples);		

	// Send collected data to GUI
	if (ui_on && (!listen)) notifyCursor ();

	// Close off sequence
	lv2_atom_forge_pop (&forge, &frame);
}

void BAngr::play (const uint32_t start, const uint32_t end)
{
	for (uint32_t i = start; i < end; ++i)
	{
		// Update cursor
		if (!listen)
		{
			if (count >= rate)
			{
				dspeedrand = bidist (rnd) * controllers[SPEED_RANGE] - speedrand;
				dspinrand = bidist (rnd) * controllers[SPIN_RANGE] - spinrand;
				count = 0.0;
			}

			else count++;

			float dspeedflex;
			float dspinflex;
			const int speedtype = controllers[SPEED_TYPE];
			const int spintype = controllers[SPIN_TYPE];

			// Filter lows, mids, highs for level calculation
			std::array<float, NR_FLEX> s;
			s.fill (0.5f * (audioInput1[i] + audioInput2[i]));
			if ((speedtype == LOWS) || (spintype == LOWS)) s[LOWS] = lowpassFilter.process (s[LOWS]);
			if ((speedtype == MIDS) || (spintype == MIDS)) s[MIDS] = bandpassFilter.process (s[MIDS]);
			if ((speedtype == HIGHS) || (spintype == HIGHS)) s[HIGHS] = highpassFilter.process (s[HIGHS]);

			// Calculate change in speed flexibility
			if (speedtype == RANDOM) dspeedflex = dspeedrand * (1.0f / (flexTime[speedtype] * rate));
			else 
			{
				// Calculate level
				const float speedDb = CO2DB (0.5f * (fabsf (s[speedtype])));
				speedlevel = (1.0 - 1.0 / (flexTime[LEVEL] * rate)) * speedlevel + 1.0 / (flexTime[LEVEL] * rate) * (LIMIT (speedDb, -50.0f, -10.0f) + 50.0f) / 40.0f;

				dspeedflex = (2.0f * speedlevel - 1.0f) * controllers[SPEED_RANGE] - speedflex;
			}

			// Calculate change in spin flexibility
			if (spintype == RANDOM) dspinflex =  dspinrand * (1.0f / (flexTime[spintype] * rate));
			else
			{
				// Calculate level
				const float spinDb = CO2DB (0.5 * (fabsf (s[spintype])));
				spinlevel = (1.0 - 1.0 / (flexTime[LEVEL] * rate)) * spinlevel + 1.0 / (flexTime[LEVEL] * rate) * (LIMIT (spinDb, -50.0f, -10.0f) + 50.0f) / 40.0f;

				if (spinlevel < 0.001f) spindir = (bidist (rnd) >= 0.0f ? 1.0f : -1.0f);
				dspinflex = spindir * spinlevel * controllers[SPIN_RANGE] - spinflex;
			}; 

			// Update speed
			speedrand += (1.0f / rate) * dspeedrand;
			speedflex += dspeedflex;
			speed = controllers[SPEED] + controllers[SPEED_AMOUNT] * speedflex + (1.0f - controllers[SPEED_AMOUNT]) * speedrand;
			speed = LIMIT (speed, 0.0f, 1.0f);

			// Update ang
			spinrand += (1.0f / rate) * dspinrand;
			spinflex += dspinflex;
			spin = controllers[SPIN] + controllers[SPIN_AMOUNT] * spinflex + (1.0f - controllers[SPIN_AMOUNT]) * spinrand;
			spin = LIMIT (spin, -1.0f, 1.0f);
			ang += 2.0 * M_PI * (10.0 / rate) * spin;

			// Calulate new positions
			const float dx = sinf (ang);
			const float dy = cosf (ang);
			xcursor += dx * (1.0 / rate) * speed * speed;
			ycursor += dy * (1.0 / rate) * speed * speed;

			// Reflections
			if (xcursor < 0.0f)
			{
				xcursor = 0.0f;
				if (dx < 0.0f) {ang = -ang; spin = 0.0f;}
			}

			else if (xcursor > 1.0f)
			{
				xcursor = 1.0f;
				if (dx > 0.0f) {ang = -ang; spin = 0.0f;}
			}

			if (ycursor < 0.0f)
			{
				ycursor = 0.0f;
				if (dy < 0.0f) {ang = -M_PI - ang; spin = 0.0f;}
			}

			else if (ycursor > 1.0f)
			{
				ycursor = 1.0f;
				if (dy > 0.0f) {ang = -M_PI - ang; spin = 0.0f;}
			}
		}

		// Calculate params for cursor position
		float params[NR_PARAMS] = {0};
		for (int i = 0; i < NR_PARAMS; ++i)
		{
			params[i] =
			(
				((1.0f - xcursor) * (1.0f - ycursor) * controllers[FX + 0 * NR_PARAMS + i]) +
				((1.0f - xcursor) * ycursor * controllers[FX + 1 * NR_PARAMS + i]) +
				(xcursor * ycursor * controllers[FX + 2 * NR_PARAMS + i]) +
				(xcursor * (1.0 - ycursor) * controllers[FX + 3 * NR_PARAMS + i])
			);
		}

		// Update fader
		if (fader != (1.0f - controllers[BYPASS]))
		{
			fader += (fader < 1.0f - controllers[BYPASS] ? 100.0 / rate : -100.0 / rate);
			fader = LIMIT (fader, 0.0f, 1.0f);
		}

		// Set params and process
		if (fader)
		{
			float out1 = audioOutput1[i];
			float out2 = audioOutput2[i];
			xregion.setParameters (params);
			xregion.process (&audioInput1[i], &audioInput2[i], &out1, &out2, 1);

			// Dry/wet mix
			audioOutput1[i] = fader * controllers[DRY_WET] * out1 + (1.0f - fader * controllers[DRY_WET]) * audioInput1[i];
			audioOutput2[i] = fader * controllers[DRY_WET] * out2 + (1.0f - fader * controllers[DRY_WET]) * audioInput2[i];
		}

		else
		{
			audioOutput1[i] = audioInput1[i];
			audioOutput2[i] = audioInput2[i];

		}
	}
}

void BAngr::notifyCursor()
{
	LV2_Atom_Forge_Frame frame;
	lv2_atom_forge_frame_time(&forge, 0);
	lv2_atom_forge_object(&forge, &frame, 0, urids.bangr_cursorEvent);
	lv2_atom_forge_key(&forge, urids.bangr_xcursor);
	lv2_atom_forge_float(&forge, xcursor);
	lv2_atom_forge_key(&forge, urids.bangr_ycursor);
	lv2_atom_forge_float(&forge, ycursor);
	lv2_atom_forge_pop(&forge, &frame);
}

static LV2_Handle instantiate (const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
	// New instance
	BAngr* instance;
	try {instance = new BAngr(samplerate, features);}
	catch (std::exception& exc)
	{
		fprintf (stderr, "BAngr.lv2: Plugin instantiation failed. %s\n", exc.what ());
		return NULL;
	}

	if (!instance->map)
	{
		fprintf(stderr, "BAngr.lv2: Host does not support urid:map.\n");
		delete (instance);
		return NULL;
	}

	return (LV2_Handle)instance;
}

static void connect_port (LV2_Handle instance, uint32_t port, void *data)
{
	BAngr* inst = (BAngr*) instance;
	if (inst) inst->connect_port (port, data);
}

static void run (LV2_Handle instance, uint32_t n_samples)
{
	BAngr* inst = (BAngr*) instance;
	if (inst) inst->run (n_samples);
}

static void cleanup (LV2_Handle instance)
{
	BAngr* inst = (BAngr*) instance;
	if (inst) delete inst;
}

static const LV2_Descriptor descriptor =
{
		BANGR_URI,
		instantiate,
		connect_port,
		NULL, //activate,
		run,
		NULL, //deactivate,
		cleanup,
		NULL //extension_data
};

// LV2 Symbol Export
LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
	switch (index)
	{
	case 0: return &descriptor;
	default: return NULL;
	}
}
