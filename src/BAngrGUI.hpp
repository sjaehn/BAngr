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

#ifndef BANGRGUI_HPP_
#define BANGRGUI_HPP_

#include <array>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include "BWidgets/Widget.hpp"
#include "BWidgets/Window.hpp"
#include "BWidgets/Label.hpp"
#include "Dial.hpp"
#include "DialRange.hpp"
#include "Dot.hpp"
#include "Definitions.hpp"
#include "Ports.hpp"
#include "Urids.hpp"

#ifdef LOCALEFILE
#include LOCALEFILE
#else
#include "Locale_EN.hpp"
#endif

#ifdef SKINFILE
#include SKINFILE
#else
#include "Skin_Default.hpp"
#endif

#ifndef LIMIT
#define LIMIT(g , max) ((g) > (max) ? (max) : (g))
#endif

#ifndef WWW_BROWSER_CMD
#define WWW_BROWSER_CMD "x-www-browser"
#endif

#define XREGION_URL "http://www.airwindows.com/xregion/"
#define RESIZE(widget, x, y, w, h, sz) (widget).moveTo ((x) * (sz), (y) * (sz)); (widget).resize ((w) * (sz), (h) * (sz));

class BAngrGUI : public BWidgets::Window
{
public:
	BAngrGUI (const char *bundle_path, const LV2_Feature *const *features, PuglNativeView parentWindow);
	~BAngrGUI ();
	void portEvent (uint32_t port_index, uint32_t buffer_size, uint32_t format, const void *buffer);
	void sendUiOn ();
	void sendUiOff ();
	void sendCursor ();
	void sendCursorOff ();
	virtual void onConfigureRequest (BEvents::ExposeEvent* event) override;
	void applyTheme (BStyles::Theme& theme) override;

	LV2UI_Controller controller;
	LV2UI_Write_Function write_function;


private:
	void resizeGUI ();
	static void valueChangedCallback (BEvents::Event* event);
	static void cursorDraggedCallback (BEvents::Event* event);
	static void cursorReleasedCallback (BEvents::Event* event);
	static void xregionClickedCallback (BEvents::Event* event);

	std::string pluginPath;
	double sz;
	cairo_surface_t* bgImageSurface;

	LV2_Atom_Forge forge;
	BAngrURIs urids;
	LV2_URID_Map* map;

	// Widgets
	BWidgets::Widget mContainer;
	Dot cursor;
	BWidgets::Label poweredLabel;
	DialRange speedDial;
	BWidgets::Label speedLabel;
	DialRange spinDial;
	BWidgets::Label spinLabel;

	struct Fx
	{
		BWidgets::Widget container;
		std::array<Dial, NR_PARAMS> paramDials;
		std::array<BWidgets::Label, NR_PARAMS> paramLabels;
	};
	std::array<Fx, NR_FX> fx;

	// Controllers
	std::array<BWidgets::ValueWidget*, NR_CONTROLLERS> controllerWidgets;

	// Definition of styles
	BColors::ColorSet fgColors = BANGR_FG_COLORS;
	BColors::ColorSet txColors = BANGR_TX_COLORS;
	BColors::ColorSet bgColors = BANGR_BG_COLORS;

	//BStyles::Border border = {{ink, 1.0}, 0.0, 2.0, 0.0};
	BStyles::Fill widgetBg = BStyles::noFill;
	BStyles::Fill screenBg = BStyles::Fill (BColors::Color (BANGR_SCREEN_BG_COLORS));
	//BStyles::Border screenBorder = BStyles::Border (BStyles::Line (BColors::Color (0.0, 0.0, 0.0, 0.75), 4.0));
	BStyles::Font defaultFont =	BStyles::Font 
	(
		"Sans", 
		CAIRO_FONT_SLANT_NORMAL, 
		CAIRO_FONT_WEIGHT_NORMAL, 
		12.0,
		BStyles::TEXT_ALIGN_CENTER, 
		BStyles::TEXT_VALIGN_MIDDLE
	);

	BStyles::StyleSet defaultStyles = {"default", {{"background", STYLEPTR (&BStyles::noFill)}, {"border", STYLEPTR (&BStyles::noBorder)}}};
	BStyles::StyleSet labelStyles = {"labels", {{"background", STYLEPTR (&BStyles::noFill)},
						    {"border", STYLEPTR (&BStyles::noBorder)},
						    {"textcolors", STYLEPTR (&txColors)},
						    {"font", STYLEPTR (&defaultFont)}}};

	BStyles::StyleSet focusStyles = {"labels", {{"background", STYLEPTR (&screenBg)},
					{"border", STYLEPTR (&BStyles::noBorder)},
					{"textcolors", STYLEPTR (&fgColors)},
					{"font", STYLEPTR (&defaultFont)}}};

	BStyles::Theme theme = BStyles::Theme 
	({
		defaultStyles,
		{"B.Angr", 		{{"background", STYLEPTR (&BStyles::blackFill)},
						 {"border", STYLEPTR (&BStyles::noBorder)}}},
		{"main", 		{{"background", STYLEPTR (&widgetBg)},
				 		 {"border", STYLEPTR (&BStyles::noBorder)}}},
		{"widget", 		{{"uses", STYLEPTR (&defaultStyles)}}},
		{"dial", 		{{"uses", STYLEPTR (&defaultStyles)},
						 {"fgcolors", STYLEPTR (&fgColors)},
						 {"bgcolors", STYLEPTR (&bgColors)},
						 {"textcolors", STYLEPTR (&txColors)},
						 {"font", STYLEPTR (&defaultFont)}}},
		{"dial/focus", 	{{"uses", STYLEPTR (&focusStyles)}}},
		{"label",	 	{{"uses", STYLEPTR (&labelStyles)}}},
		{"dot", 		{{"uses", STYLEPTR (&defaultStyles)},
						 {"fgcolors", STYLEPTR (&txColors)}}}
	});
};

#endif /* BANGRGUI_HPP_ */