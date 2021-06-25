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

#ifndef URIDS_HPP_
#define URIDS_HPP_

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include <lv2/lv2plug.in/ns/ext/time/time.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include "Definitions.hpp"

struct BAngrURIs
{
	LV2_URID atom_Float;
	LV2_URID atom_Object;
	LV2_URID atom_Blank;
	LV2_URID atom_eventTransfer;
	LV2_URID bangr_uiOn;
	LV2_URID bangr_uiOff;
	LV2_URID bangr_cursorEvent;
	LV2_URID bangr_xcursor;
	LV2_URID bangr_ycursor;
};

void getURIs (LV2_URID_Map* m, BAngrURIs* uris)
{
	uris->atom_Float = m->map(m->handle, LV2_ATOM__Float);
	uris->atom_Object = m->map(m->handle, LV2_ATOM__Object);
	uris->atom_Blank = m->map(m->handle, LV2_ATOM__Blank);
	uris->atom_eventTransfer = m->map(m->handle, LV2_ATOM__eventTransfer);
	uris->bangr_uiOn = m->map(m->handle, BANGR_URI "#uiOn");
	uris->bangr_uiOff = m->map(m->handle, BANGR_URI "#uiOff");
	uris->bangr_cursorEvent = m->map(m->handle, BANGR_URI "#cursorEvent");
	uris->bangr_xcursor = m->map(m->handle, BANGR_URI "#xcursor");
	uris->bangr_ycursor = m->map(m->handle, BANGR_URI "#ycursor");
}

#endif /* URIDS_HPP_ */
