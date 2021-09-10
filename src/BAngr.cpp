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
#include <stdexcept>
#include "ControllerLimits.hpp"

#define LIMIT(g , min, max) ((g) > (max) ? (max) : ((g) < (min) ? (min) : (g)))

const float flexTime[NR_FLEX] = {1.0f, 0.05f, 0.05f, 0.05f, 0.05f};

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
	speedmaxlevel (0.1f),
	spinlevel (0.0f),
	spinmaxlevel (0.1f),
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

			if (obj->body.otype == urids.bangr_cursorOn) listen = true;
			else if (obj->body.otype == urids.bangr_cursorOff) listen = false;

			else if (obj->body.otype == urids.patch_Set)
			{
				const LV2_Atom* property = NULL;
      			const LV2_Atom* value    = NULL;
				lv2_atom_object_get
				(
					obj,
                    urids.patch_property, &property,
                    urids.patch_value, &value,
                    NULL
				);

				if (property && (property->type == urids.atom_URID) && value)
				{
					const uint32_t key = ((const LV2_Atom_URID*)property)->body;
					
					if ((key == urids.bangr_xcursor) && (value->type == urids.atom_Float)) xcursor = ((LV2_Atom_Float*)value)->body;
					else if ((key == urids.bangr_ycursor) && (value->type == urids.atom_Float)) ycursor = ((LV2_Atom_Float*)value)->body;
				}
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
	if (!listen) notifyCursor ();

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
				const float coeff = fabsf (s[speedtype]);
				if (coeff >= speedmaxlevel) speedmaxlevel = coeff;
				else speedmaxlevel = (1.0 - 1.0 / (4.0 * rate)) * speedmaxlevel;
				if (speedmaxlevel < 0.0001f) speedmaxlevel = 0.0001f;	// Limit to -80 db to prevent div by zero
				speedlevel = (1.0 - 1.0 / (flexTime[LEVEL] * rate)) * speedlevel + 1.0 / (flexTime[LEVEL] * rate) * (2.0 * coeff / speedmaxlevel);

				dspeedflex = (2.0f * LIMIT (speedlevel, 0.0f, 1.0f) - 1.0f) * controllers[SPEED_RANGE] - speedflex;

				// Command line meter (for debugging)
				/*
				if (int(count) % 1000 == 0) 
				{
					char s[41];
					memset (s, ' ', 40);
					int x = int (40 * LIMIT (speedlevel, 0.0f, 1.0f));
					memset (s, '.', x);
					s[40] = 0;
					fprintf (stderr, "\r%s", s);
				}
				*/
			}

			// Calculate change in spin flexibility
			if (spintype == RANDOM) dspinflex =  dspinrand * (1.0f / (flexTime[spintype] * rate));
			else
			{
				// Calculate level
				const float coeff = fabsf (s[spintype]);
				if (coeff >= spinmaxlevel) spinmaxlevel = coeff;
				else spinmaxlevel = (1.0 - 1.0 / (4.0 * rate)) * spinmaxlevel;
				if (spinmaxlevel < 0.0001f) spinmaxlevel = 0.0001f;	// Limit to -80 db to prevent div by zero
				const float nspinlevel = (1.0 - 1.0 / (flexTime[LEVEL] * rate)) * spinlevel + 1.0 / (flexTime[LEVEL] * rate) * (2.0 * coeff / spinmaxlevel);

				if ((spinlevel >= 0.2f) && (nspinlevel < 0.2f)) spindir = (bidist (rnd) >= 0.0f ? spindir : -spindir);
				spinlevel = nspinlevel;
				dspinflex = spindir * LIMIT (spinlevel, 0.0f, 1.0f) * controllers[SPIN_RANGE] - spinflex;
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
	lv2_atom_forge_object(&forge, &frame, 0, urids.patch_Set);
	lv2_atom_forge_key(&forge, urids.patch_property);
	lv2_atom_forge_urid(&forge, urids.bangr_xcursor);
	lv2_atom_forge_key(&forge, urids.patch_value);
	lv2_atom_forge_float(&forge, xcursor);
	lv2_atom_forge_pop(&forge, &frame);

	lv2_atom_forge_frame_time(&forge, 0);
	lv2_atom_forge_object(&forge, &frame, 0, urids.patch_Set);
	lv2_atom_forge_key(&forge, urids.patch_property);
	lv2_atom_forge_urid(&forge, urids.bangr_ycursor);
	lv2_atom_forge_key(&forge, urids.patch_value);
	lv2_atom_forge_float(&forge, ycursor);
	lv2_atom_forge_pop(&forge, &frame);
}

LV2_State_Status BAngr::state_save (LV2_State_Store_Function store, LV2_State_Handle handle, uint32_t flags,
			const LV2_Feature* const* features)
{
	store (handle, urids.bangr_xcursor, &xcursor, sizeof (float), urids.atom_Float, LV2_STATE_IS_POD);
	store (handle, urids.bangr_ycursor, &ycursor, sizeof (float), urids.atom_Float, LV2_STATE_IS_POD);
	return LV2_STATE_SUCCESS;
}

LV2_State_Status BAngr::state_restore (LV2_State_Retrieve_Function retrieve, LV2_State_Handle handle, uint32_t flags,
			const LV2_Feature* const* features)
{
	size_t   size;
	uint32_t type;
	uint32_t valflags;

	const void* xcursorData = retrieve(handle, urids.bangr_xcursor, &size, &type, &valflags);
	if (xcursorData && (type == urids.atom_Float)) xcursor = LIMIT (*(const float*)xcursorData, 0.0f, 1.0f);

	const void* ycursorData = retrieve(handle, urids.bangr_ycursor, &size, &type, &valflags);
	if (ycursorData && (type == urids.atom_Float)) ycursor = LIMIT (*(const float*)ycursorData, 0.0f, 1.0f);

	return LV2_STATE_SUCCESS;
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

static LV2_State_Status state_save(LV2_Handle instance, LV2_State_Store_Function store, LV2_State_Handle handle, uint32_t flags,
           const LV2_Feature* const* features)
{
	BAngr* inst = (BAngr*)instance;
	if (!inst) return LV2_STATE_SUCCESS;

	inst->state_save (store, handle, flags, features);
	return LV2_STATE_SUCCESS;
}

static LV2_State_Status state_restore(LV2_Handle instance, LV2_State_Retrieve_Function retrieve, LV2_State_Handle handle, uint32_t flags,
           const LV2_Feature* const* features)
{
	BAngr* inst = (BAngr*)instance;
	if (inst) inst->state_restore (retrieve, handle, flags, features);
	return LV2_STATE_SUCCESS;
}

static const void* extension_data(const char* uri)
{
	static const LV2_State_Interface  state  = {state_save, state_restore};
	if (!strcmp(uri, LV2_STATE__interface)) return &state;
	return NULL;
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
		extension_data
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
