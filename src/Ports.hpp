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

#ifndef PORTS_HPP_
#define PORTS_HPP_

enum BAngrPortIndex
{
	CONTROL		= 0,
	NOTIFY		= 1,
	AUDIO_IN_1	= 2,
	AUDIO_IN_2	= 3,
	AUDIO_OUT_1	= 4,
	AUDIO_OUT_2	= 5,

	CONTROLLERS	= 6,
	BYPASS		= 0,
	DRY_WET		= 1,
	SPEED		= 2,
	SPEED_RANGE	= 3,
	SPEED_TYPE	= 4,
	SPEED_AMOUNT= 5,
	SPIN		= 6,
	SPIN_RANGE	= 7,
	SPIN_TYPE	= 8,
	SPIN_AMOUNT	= 9,
	FX			= 10,
	NR_FX		= 4,

	PARAM_GAIN	= 0,
	PARAM_FIRST	= 1,
	PARAM_LAST	= 2,
	PARAM_NUKE	= 3,
	PARAM_MIX	= 4,
	PARAM_PAN	= 5,
	NR_PARAMS	= 6,

	NR_CONTROLLERS	= FX + NR_FX * NR_PARAMS
};

enum BAngrFlexibilityIndex
{
	RANDOM		= 0,
	LEVEL		= 1,
	LOWS		= 2,
	MIDS		= 3,
	HIGHS		= 4,
	NR_FLEX		= 5
};

#endif /* PORTS_HPP_ */
