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

#ifndef BANGR_HPP_
#define BANGR_HPP_

#include <cmath>
#include <ctime>
#include <array>
#include <random>
#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include <lv2/lv2plug.in/ns/ext/state/state.h>

#include "Definitions.hpp"
#include "Ports.hpp"
#include "Urids.hpp"
#include "Airwindows/XRegion.hpp"
#include "ButterworthLowPassFilter.hpp"
#include "ButterworthHighPassFilter.hpp"
#include "ButterworthBandPassFilter.hpp"


class BAngr
{
public:
	BAngr (double samplerate, const LV2_Feature* const* features);
	~BAngr();

	void connect_port (uint32_t port, void *data);
	void run (uint32_t n_samples);
	LV2_State_Status state_save(LV2_State_Store_Function store, LV2_State_Handle handle, uint32_t flags, const LV2_Feature* const* features);
	LV2_State_Status state_restore(LV2_State_Retrieve_Function retrieve, LV2_State_Handle handle, uint32_t flags, const LV2_Feature* const* features);

	LV2_URID_Map* map;

private:
	double rate;
	float xcursor;
	float ycursor;
	bool listen;
	std::minstd_rand rnd;
	std::uniform_real_distribution<float> bidist;
	double count;
	float fader;
	float speed;
	float speedrand;
	float dspeedrand;
	float speedflex;
	float spin;
	float spinrand;
	float dspinrand;
	float spinflex;
	float spindir;
	float ang;
	float speedlevel;
	float speedmaxlevel;
	float spinlevel;
	float spinmaxlevel;
	ButterworthLowPassFilter lowpassFilter;
	ButterworthHighPassFilter highpassFilter;
	ButterworthBandPassFilter bandpassFilter;

	// Control ports
	LV2_Atom_Sequence* controlPort;
	LV2_Atom_Sequence* notifyPort;

	// Audio ports
	float* audioInput1;
	float* audioInput2;
	float* audioOutput1;
	float* audioOutput2;

	// Controllers
	float* newControllers[NR_CONTROLLERS];
	float controllers[NR_CONTROLLERS];

	// Urids
	BAngrURIs urids;

	// Internals
	LV2_Atom_Forge forge;
	LV2_Atom_Forge_Frame frame;
	XRegion xregion;

	void play (const uint32_t start, const uint32_t end);
	void notifyCursor();

};

#endif /* BANGR_HPP_ */
