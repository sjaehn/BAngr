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
	SPEED		= 0,
	SPEED_RANGE	= 1,
	SPIN		= 2,
	SPIN_RANGE	= 3,
	FX			= 4,
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

#endif /* PORTS_HPP_ */
