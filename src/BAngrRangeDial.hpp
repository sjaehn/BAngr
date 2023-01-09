/* B.Angr
 * Dynamic distorted bandpass filter plugin
 *
 * Copyright (C) 2021 - 2023 by Sven Jähnichen
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

#ifndef BANGRRANGEDIAL_HPP_
#define BANGRRANGEDIAL_HPP_

#include "BAngrDial.hpp"
#include "BWidgets/BEvents/Event.hpp"
#include "BWidgets/BEvents/PointerEvent.hpp"
#include "BWidgets/BStyles/Types/Color.hpp"
#include "BWidgets/BWidgets/Dial.hpp"
#include "BWidgets/BWidgets/Frame.hpp"
#include "BWidgets/BWidgets/Supports/ValueableTyped.hpp"
#include <cairo/cairo.h>
#include <cstdlib>
#include <utility>

class BAngrRangeDial :	public BAngrDial
{
public:
	BWidgets::Dial range;

	BAngrRangeDial ();
	BAngrRangeDial (const uint32_t urid, const std::string& title);
	BAngrRangeDial	(const double value, const double range, const double min, const double max, double step = 0.0,
					 uint32_t urid = BUTILITIES_URID_UNKNOWN_URID, std::string title = "");
	BAngrRangeDial	(const double x, const double y, const double width, const double height, 
					 const double value, const double range, const double min, const double max, double step = 0.0,
					 std::function<double (const double& x)> transferFunc = ValueTransferable<double>::noTransfer,
					 std::function<double (const double& x)> reTransferFunc = ValueTransferable<double>::noTransfer,
					 std::function<std::string (const double& x)> displayFunc = BAngrDial::valueToString,
					 std::function<double (const std::string& s)> reDisplayFunc = BAngrDial::stringToValue,
					 uint32_t urid = BUTILITIES_URID_UNKNOWN_URID, std::string title = "");

	virtual Widget* clone () const override; 
	void copy (const BAngrRangeDial* that);
	virtual void setMin (const double& min) override;
	virtual void setMax (const double& max) override;
	virtual void setStep (const double& step) override;
	virtual void setRange (const double& min, const double& max) override;
	virtual void setRange (const double& min, const double& max, const double& step) override;
	virtual void update () override;
    virtual void onPointerDragged (BEvents::Event* event) override;
    virtual void onWheelScrolled (BEvents::Event* event) override;

protected:
	static void rangeChangedCallback (BEvents::Event* event);
	virtual void draw () override;
    virtual void draw (const double x0, const double y0, const double width, const double height) override;
    virtual void draw (const BUtilities::Area<>& area) override;

};

inline BAngrRangeDial::BAngrRangeDial () :
	BAngrRangeDial	(0.0, 0.0, 0.0, 0.0, 
					 0.0, 0.0, 0.0, 1.0, 0.0, 
					 ValueTransferable<double>::noTransfer, ValueTransferable<double>::noTransfer,
				 	 BAngrDial::valueToString, BAngrDial::stringToValue, 
			 		 BUTILITIES_URID_UNKNOWN_URID, "")
{

}



inline BAngrRangeDial::BAngrRangeDial (const uint32_t urid, const std::string& title) : 
	BAngrRangeDial	(0.0, 0.0, 0.0, 0.0, 
					 0.0, 0.0, 0.0, 1.0, 0.0, 
					 ValueTransferable<double>::noTransfer, ValueTransferable<double>::noTransfer, 
				 	 BAngrDial::valueToString, BAngrDial::stringToValue, 
					 urid, title) 
{

}

inline BAngrRangeDial::BAngrRangeDial (const double value, const double range, const double min, const double max, double step, uint32_t urid, std::string title) : 
	BAngrRangeDial	(0.0, 0.0, 0.0, 0.0, 
					 value, range, min, max, step,
					 ValueTransferable<double>::noTransfer, ValueTransferable<double>::noTransfer,
				 	 BAngrDial::valueToString, BAngrDial::stringToValue,  
					 urid, title) 
{

}

inline BAngrRangeDial::BAngrRangeDial	(const double  x, const double y, const double width, const double height, 
										 const double value, const double range, const double min, const double max, double step,
										 std::function<double (const double& x)> transferFunc,
							 			 std::function<double (const double& x)> reTransferFunc,
										 std::function<std::string (const double& x)> displayFunc,
										 std::function<double (const std::string& s)> reDisplayFunc,
										 uint32_t urid, std::string title) :
	BAngrDial (x, y, width, height, value, min, max, step, transferFunc, reTransferFunc, displayFunc, reDisplayFunc, urid, title), 
	range (0, 0, 0, 0, range, min, max, step)
{
	this->range.hide();
	this->range.setCallbackFunction(BEvents::Event::VALUE_CHANGED_EVENT, rangeChangedCallback);
	add (&this->range);
}

inline BWidgets::Widget* BAngrRangeDial::clone () const 
{
	Widget* f = new BAngrRangeDial (urid_, title_);
	f->copy (this);
	return f;
}

inline void BAngrRangeDial::copy (const BAngrRangeDial* that)
{
	range.copy (&that->range);
	BAngrDial::copy (that);
}


inline void BAngrRangeDial::setMin (const double& min)
{
	range.setMin(min);
	BAngrDial::setMin(min);
}
inline void BAngrRangeDial::setMax (const double& max)
{
	range.setMax(max);
	BAngrDial::setMax(max);
}

inline void BAngrRangeDial::setStep (const double& step) 
{
	range.setStep(step);
	BAngrDial::setStep(step);
}

inline void BAngrRangeDial::setRange (const double& min, const double& max)
{
	range.setRange(min, max);
	BAngrDial::setRange(min, max);
}

inline void BAngrRangeDial::setRange (const double& min, const double& max, const double& step)
{
	range.setRange(min, max, step);
	BAngrDial::setRange(min, max, step);
}

inline void BAngrRangeDial::update ()
{
	BWidgets::Label* f = dynamic_cast<BWidgets::Label*>(focus_);
	if (f)
	{
		f->setText(getTitle() + ": " + std::to_string (this->getValue()));
		f->resize();
	}

	const bool lv = label.isValueable();
	label.setValueable (false);
	label.setText (display_ (getValue()));
	label.setValueable (lv);
	label.resize();
	label.moveTo(label.center(), label.middle());

	const double x0 = getXOffset ();
	const double y0 = getYOffset ();
	const double w = getEffectiveWidth ();
	const double h = getEffectiveHeight ();
	const double ext = std::min (w, h);
	const double xm = x0 + 0.5 * w;
	const double ym = y0 + 0.5 * h;
	scale_ = BUtilities::Area<> (xm - 0.4 * ext, ym - 0.4 * ext, 0.8 * ext, 0.8 * ext);
	Widget::update();
}

inline void BAngrRangeDial::onPointerDragged (BEvents::Event* event)
{
	BEvents::PointerEvent* pev = dynamic_cast<BEvents::PointerEvent*> (event);
	if (!pev) return;

	const double x0 = getXOffset ();
	const double y0 = getYOffset ();
	const double w = getEffectiveWidth ();
	const double h = getEffectiveHeight ();
	const double ext = std::min (w, h);
	const double xm = x0 + 0.5 * w;
	const double ym = y0 + 0.5 * h;
	const double doxm = pev->getOrigin().x - xm;
	const double doym = pev->getOrigin().y - ym;
	const double r = sqrt (doxm * doxm + doym * doym);

	// Inside dial arc: Forward event to dial
	if (r <= 0.4 * ext) BAngrDial::onPointerDragged(event);

	// Outside dial arc: Drag handles
	else 
	{
		if (getEffectiveWidth() >= 1) 
		{
			if (range.getStep() != 0.0) range.setValue (range.getValue() - pev->getDelta().y * range.getStep ());
			else range.setValue (range.getValueFromRatio (range.getRatioFromValue (range.getValue()) - pev->getDelta().y / 
														  (0.5 * scale_.getWidth() * BWIDGETS_DEFAULT_DRAWARC_SIZE)));
		}
		Draggable::onPointerDragged (event);
	}
}

inline void BAngrRangeDial::onWheelScrolled (BEvents::Event* event)
{
	BEvents::WheelEvent* wev = dynamic_cast<BEvents::WheelEvent*> (event);
	if (!wev) return;

	const double x0 = getXOffset ();
	const double y0 = getYOffset ();
	const double w = getEffectiveWidth ();
	const double h = getEffectiveHeight ();
	const double ext = std::min (w, h);
	const double xm = x0 + 0.5 * w;
	const double ym = y0 + 0.5 * h;
	const double doxm = wev->getPosition().x - xm;
	const double doym = wev->getPosition().y - ym;
	const double r = sqrt (doxm * doxm + doym * doym);

	// Inside dial arc: Forward event to dial
	if (r <= 0.4 * ext) BAngrDial::onWheelScrolled(event);

	// Outside dial arc: Scroll handles
	else 
	{
		if (getEffectiveWidth() >= 1) 
		{
			if (range.getStep() != 0.0) range.setValue (range.getValue() - wev->getDelta().y * range.getStep ());
			else range.setValue (range.getValueFromRatio (range.getRatioFromValue (range.getValue()) - wev->getDelta().y / 
														  (0.5 * scale_.getWidth() * BWIDGETS_DEFAULT_DRAWARC_SIZE)));
		}
		Scrollable::onWheelScrolled (event);
	}
}

inline void BAngrRangeDial::rangeChangedCallback (BEvents::Event* event)
{
	BEvents::ValueChangeTypedEvent<double>* vev = dynamic_cast<BEvents::ValueChangeTypedEvent<double>*>(event);
	if (!vev) return;

	BAngrDial* w = dynamic_cast<BAngrDial*>(vev->getWidget());
	if (!w) return;

	BAngrRangeDial* p = dynamic_cast<BAngrRangeDial*>(w->getParentWidget());
	if (!p) return;

	p->update();
}

inline void BAngrRangeDial::draw ()
{
	draw (0, 0, getWidth(), getHeight());
}

inline void BAngrRangeDial::draw (const double x0, const double y0, const double width, const double height)
{
	draw (BUtilities::Area<> (x0, y0, width, height));
}

inline void BAngrRangeDial::draw (const BUtilities::Area<>& area)
{
	if ((!cairoSurface()) || (cairo_surface_status (cairoSurface()) != CAIRO_STATUS_SUCCESS)) return;

	// Draw super class widget elements first
	BAngrDial::draw (area);

	const double x0 = getXOffset ();
	const double y0 = getYOffset ();
	const double h = getEffectiveHeight ();
	const double w = getEffectiveWidth ();
	const double d = (w < h ? w : h);
	const double xc = x0 + 0.5 * w;
	const double yc = y0 + 0.5 * h;

	// Draw scale only if it is not a null widget
	if (d >= 1)
	{
		cairo_t* cr = cairo_create (cairoSurface());

		if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
		{
			// Limit cairo-drawing area
			cairo_rectangle (cr, area.getX (), area.getY (), area.getWidth (), area.getHeight ());
			cairo_clip (cr);

			const double v1 = std::max (std::min (getValue() - range.getValue(), getMax()), getMin());
			const double v2 = std::max (std::min (getValue() + range.getValue(), getMax()), getMin());
			double rv1 = getRatioFromValue(v1);
			double rv2 = getRatioFromValue(v2);
			if (rv2 < rv1) std::swap (rv1, rv2);
			const double p1 = BWIDGETS_DEFAULT_DRAWARC_START + BWIDGETS_DEFAULT_DRAWARC_SIZE * rv1;
			const double p2 = BWIDGETS_DEFAULT_DRAWARC_START + BWIDGETS_DEFAULT_DRAWARC_SIZE * rv2;
			const BStyles::Color fgColor = BStyles::white;

			cairo_new_path (cr);

			// Arc
			cairo_set_source_rgba (cr, CAIRO_RGBA (fgColor));
			cairo_set_line_width (cr, 0.0);
			cairo_arc (cr, xc, yc, 0.46 * d, p1, p2);
			cairo_arc_negative (cr, xc, yc ,  0.48 * d, p2, p1);
			cairo_close_path (cr);
			cairo_fill (cr);

			// Arrows
			cairo_save (cr);
			
			cairo_translate (cr, xc, yc);
			cairo_rotate (cr, p1);
			cairo_move_to (cr, 0.5 * d, 0);
			cairo_rel_line_to (cr, -0.06 * d, 0);
			cairo_rel_line_to (cr, 0.03 * d, -0.04 * d);
			cairo_close_path (cr);
			cairo_fill (cr);
			
			cairo_restore (cr);
			cairo_translate (cr, xc, yc);
			cairo_rotate (cr, p2);
			cairo_move_to (cr, 0.5 * d, 0);
			cairo_rel_line_to (cr, -0.06 * d, 0);
			cairo_rel_line_to (cr, 0.03 * d, 0.04 * d);
			cairo_close_path (cr);
			cairo_fill (cr);
		}
		cairo_destroy (cr);
	}
}


#endif /* BANGRRANGEDIAL_HPP_ */
