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

#ifndef CONTROLLERLIMITS_HPP_
#define CONTROLLERLIMITS_HPP_

#include "Limit.hpp"
#include "Ports.hpp"

const Limit controllerLimits [NR_CONTROLLERS] =
{
    {0.0, 1.0, 1.0},    // Bypass
    {0.0, 1.0, 0.0},    // Dry/wet
    {0.0, 1.0, 0.0},    // Speed
    {0.0, 1.0, 0.0},    // Speed range
    {-1.0, 1.0, 0.0},   // Spin
    {0.0, 2.0, 0.0},    // Spin range
    {0.0, 1.0, 0.0},    // FX1: Gain
    {0.0, 1.0, 0.0},    // First
    {0.0, 1.0, 0.0},    // Last
    {0.0, 1.0, 0.0},    // Nuke
    {0.0, 1.0, 0.0},    // Mix
    {-1.0, 1.0, 0.0},   // Pan
    {0.0, 1.0, 0.0},    // FX2: Gain
    {0.0, 1.0, 0.0},    // First
    {0.0, 1.0, 0.0},    // Last
    {0.0, 1.0, 0.0},    // Nuke
    {0.0, 1.0, 0.0},    // Mix
    {-1.0, 1.0, 0.0},   // Pan
    {0.0, 1.0, 0.0},    // FX3: Gain
    {0.0, 1.0, 0.0},    // First
    {0.0, 1.0, 0.0},    // Last
    {0.0, 1.0, 0.0},    // Nuke
    {0.0, 1.0, 0.0},    // Mix
    {-1.0, 1.0, 0.0},   // Pan
    {0.0, 1.0, 0.0},    // FX4: Gain
    {0.0, 1.0, 0.0},    // First
    {0.0, 1.0, 0.0},    // Last
    {0.0, 1.0, 0.0},    // Nuke
    {0.0, 1.0, 0.0},    // Mix
    {-1.0, 1.0, 0.0}    // Pan
};

#endif /* CONTROLLERLIMITS_HPP_ */