/* B.Angr
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2021 by Sven Jähnichen
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

#include "BWidgets/Widget.hpp"
#include <cmath>

class Dot : public BWidgets::Widget
{
public:
	Dot 	() : Dot (0, 0, 0, 0, "") {}
	Dot 	(const double x, const double y, const double width, const double height, const std::string& name) :
			Widget (x, y, width, height, name),
			fgColors_ (BWIDGETS_DEFAULT_FGCOLORS)
	{}

	virtual Widget* clone () const override {return new Dot (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		Widget::applyTheme (theme, name);

		// Foreground colors
		void* fgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_FGCOLORS);
		if (fgPtr) fgColors_ = *((BColors::ColorSet*) fgPtr);
	}

protected:
	BColors::ColorSet fgColors_;

	virtual void draw (const BUtilities::RectArea& area) override
	{
		if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

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
			cairo_surface_clear (widgetSurface_);
			cairo_t* cr = cairo_create (widgetSurface_);

			if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
			{
				// Limit cairo-drawing area
				cairo_rectangle (cr, area.getX (), area.getY (), area.getWidth (), area.getHeight ());
				cairo_clip (cr);

				BColors::Color fg = *fgColors_.getColor (getState ()); fg.applyBrightness (BWIDGETS_DEFAULT_NORMALLIGHTED);

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
