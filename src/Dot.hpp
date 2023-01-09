/* B.Angr
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2021 - 2022 by Sven Jähnichen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DOT_HPP_
#define DOT_HPP_

#include "BWidgets/BStyles/Types/Color.hpp"
#include "BWidgets/BWidgets/Frame.hpp"
#include "BWidgets/BWidgets/Supports/Clickable.hpp"
#include <cmath>

class Dot : public BWidgets::Frame,
			public BWidgets::Clickable
{
public:
	Dot () : Dot (0, 0, 0, 0, BUTILITIES_URID_UNKNOWN_URID, "") {}
	Dot (uint32_t urid = BUTILITIES_URID_UNKNOWN_URID, std::string title = "") :
			Dot (0, 0, 0, 0, urid, title) {}
	Dot (const double x, const double y, const double width, const double height, 
			uint32_t urid = BUTILITIES_URID_UNKNOWN_URID, std::string title = "") :
			Frame (x, y, width, height, urid, title),
			BWidgets::Clickable()
	{}

	virtual Widget* clone () const override 
	{
		Widget* f = new Dot (urid_, title_);
		f->copy (this);
		return f;
	}

	inline void copy (const Dot* that)
{
	Clickable::operator= (*that);
	Frame::copy (that);
}

protected:
	virtual void draw (const BUtilities::Area<>& area) override
	{
		if ((!cairoSurface()) || (cairo_surface_status (cairoSurface()) != CAIRO_STATUS_SUCCESS)) return;

		// Draw super class widget elements first
		Widget::draw (area);

		const double x0 = getXOffset ();
		const double y0 = getYOffset ();
		const double h = getEffectiveHeight ();
		const double w = getEffectiveWidth ();
		const double d = (w < h ? w : h);
		const double xc = x0 + 0.5 * w;
		const double yc = y0 + 0.5 * h;

		// Draw scale only if it is not a null widget
		if (d > 0)
		{
			cairoplus_surface_clear (cairoSurface());
			cairo_t* cr = cairo_create (cairoSurface());

			if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
			{
				// Limit cairo-drawing area
				cairo_rectangle (cr, area.getX (), area.getY (), area.getWidth (), area.getHeight ());
				cairo_clip (cr);

				BStyles::Color fg = getFgColors()[getStatus()]; 
				fg.illuminate (BStyles::Color::normalLighted);

				cairo_set_line_width (cr, 0.0);
				cairo_set_source_rgba (cr, CAIRO_RGBA (fg));
				cairo_arc (cr,xc, yc, 0.125 * d, 0.0, 2.0 * M_PI);
				cairo_close_path (cr);
				cairo_fill (cr);
			}
			cairo_destroy (cr);
		}
	}
};

#endif /* DOT_HPP_ */
