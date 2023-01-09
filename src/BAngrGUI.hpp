/* B.Angr
 * Dynamic distorted bandpass filter plugin
 *
 * Copyright (C) 2021 - 2023 by Sven Jähnichen
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

#define BWIDGETS_DEFAULT_DRAWARC_PATH "../examples/OopsDraws/drawArc.hpp"

#include <array>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include "BWidgets/BStyles/Status.hpp"
#include "BWidgets/BStyles/Types/Border.hpp"
#include "BWidgets/BStyles/Types/Color.hpp"
#include "BWidgets/BWidgets/Widget.hpp"
#include "BWidgets/BWidgets/Window.hpp"
#include "BWidgets/BWidgets/Knob.hpp"
#include "BWidgets/BWidgets/Label.hpp"
#include "BWidgets/BWidgets/ComboBox.hpp"
#include "BWidgets/BWidgets/ValueHSlider.hpp"
#include "BWidgets/BWidgets/Image.hpp"
#include "BAngrDial.hpp"
#include "BAngrRangeDial.hpp"
#include "Dot.hpp"
#include "Definitions.hpp"
#include "Ports.hpp"
#include "Urids.hpp"

#ifndef LIMIT
#define LIMIT(g , max) ((g) > (max) ? (max) : (g))
#endif

#ifndef WWW_BROWSER_CMD
#define WWW_BROWSER_CMD "x-www-browser"
#endif

#define XREGION_URL "http://www.airwindows.com/xregion/"
#define HELP_URL "https://github.com/sjaehn/BAngr/blob/master/README.md"
#define YT_URL "https://www.youtube.com/watch?v=-kWy_1UYazo"
#define URID(x) (BURID(BANGR_GUI_URI x))

class BAngrGUI : public BWidgets::Window
{
public:
	BAngrGUI (const char *bundle_path, const LV2_Feature *const *features, PuglNativeView parentWindow);
	~BAngrGUI ();
	void portEvent (uint32_t port_index, uint32_t buffer_size, uint32_t format, const void *buffer);
	void sendCursor ();
	void sendXCursor ();
	void sendYCursor ();
	void sendCursorOn ();
	void sendCursorOff ();
	virtual void onConfigureRequest (BEvents::Event* event) override;

	LV2UI_Controller controller;
	LV2UI_Write_Function write_function;


private:
	static void valueChangedCallback (BEvents::Event* event);
	static void cursorDraggedCallback (BEvents::Event* event);
	static void cursorReleasedCallback (BEvents::Event* event);
	static void xregionClickedCallback (BEvents::Event* event);
	static void helpButtonClickedCallback (BEvents::Event* event);
	static void ytButtonClickedCallback (BEvents::Event* event);

	std::string pluginPath;

	LV2_Atom_Forge forge;
	BAngrURIs urids;
	LV2_URID_Map* map;
	// LV2_URID_Unmap* unmap;

	// Widgets
	BWidgets::Image mContainer;
	Dot cursor;
	BWidgets::Label poweredLabel;
	BWidgets::Button helpButton;
	BWidgets::Button ytButton;
	BWidgets::Knob bypassButton;
	BWidgets::Label bypassLabel;
    BAngrDial drywetDial;
	BWidgets::Label drywetLabel;
	BAngrRangeDial speedDial;
	BWidgets::Label speedLabel;
	BAngrRangeDial spinDial;
	BWidgets::Label spinLabel;
	BWidgets::Widget speedScreen;
	BWidgets::Label speedFlexLabel;
	BWidgets::ComboBox speedTypeCombobox;
	BWidgets::ValueHSlider speedAmountSlider;
	BWidgets::Widget spinScreen;
	BWidgets::Label spinFlexLabel;
	BWidgets::ComboBox spinTypeCombobox;
	BWidgets::ValueHSlider spinAmountSlider;

	struct Fx
	{
		BWidgets::Widget* container;
		std::array<BAngrDial*, NR_PARAMS> paramDials;
		std::array<BWidgets::Label*, NR_PARAMS> paramLabels;
	};
	std::array<Fx, NR_FX> fx;

	// Controllers
	std::array<BWidgets::Widget*, NR_CONTROLLERS> controllerWidgets;

	// Definition of styles
	BStyles::ColorMap fgColors = BStyles::ColorMap {{{1.0, 0.0, 0.0, 1.0}, {1.0, 0.3, 0.3, 1.0}, {0.7, 0.0, 0.0, 1.0}, {0.0, 0.0, 0.0, 0.0}}};
	BStyles::ColorMap txColors = BStyles::ColorMap {{{0.8, 0.8, 0.8, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.5, 0.5, 0.5, 1.0}, {0.0, 0.0, 0.0, 0.0}}};
	BStyles::ColorMap bgColors = BStyles::ColorMap {{{0.15, 0.15, 0.15, 1.0}, {0.3, 0.3, 0.3, 1.0}, {0.05, 0.05, 0.05, 1.0}, {0.0, 0.0, 0.0, 0.0}}};
	BStyles::ColorMap noColors = BStyles::ColorMap {{{0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}}};

	BStyles::Border border = {{fgColors[BStyles::STATUS_NORMAL], 1.0}, 0.0, 2.0, 0.0};
	BStyles::Border menuBorder = {{noColors[BStyles::STATUS_NORMAL], 1.0}, 0.0, 0.0, 0.0};
	BStyles::Border menuBorder2 = {{bgColors[BStyles::STATUS_NORMAL], 1.0}, 0.0, 0.0, 0.0};
	BStyles::Border btBorder = BStyles::Border (BStyles::Line (bgColors[BStyles::Status::STATUS_NORMAL].illuminate (BStyles::Color::darkened), 1.0), 0.0, 0.0, 3.0);
	BStyles::Border labelBorder = {BStyles::noLine, 4.0, 0.0, 0.0};
	BStyles::Fill widgetBg = BStyles::noFill;
	BStyles::Fill screenBg = BStyles::Fill (BStyles::Color ({0.04, 0.0, 0.0, 0.8}));
	BStyles::Font defaultFont =	BStyles::Font 
	(
		"Sans", 
		CAIRO_FONT_SLANT_NORMAL, 
		CAIRO_FONT_WEIGHT_NORMAL, 
		12.0,
		BStyles::Font::TEXT_ALIGN_CENTER, 
		BStyles::Font::TEXT_VALIGN_MIDDLE
	);

	BStyles::Font smFont =	BStyles::Font 
	(
		"Sans", 
		CAIRO_FONT_SLANT_NORMAL, 
		CAIRO_FONT_WEIGHT_NORMAL, 
		9.0,
		BStyles::Font::TEXT_ALIGN_CENTER, 
		BStyles::Font::TEXT_VALIGN_MIDDLE
	);

	BStyles::Font rFont =	BStyles::Font 
	(
		"Sans", 
		CAIRO_FONT_SLANT_NORMAL, 
		CAIRO_FONT_WEIGHT_NORMAL, 
		12.0,
		BStyles::Font::TEXT_ALIGN_RIGHT, 
		BStyles::Font::TEXT_VALIGN_MIDDLE
	);

	BStyles::Font lFont =	BStyles::Font 
	(
		"Sans", 
		CAIRO_FONT_SLANT_NORMAL, 
		CAIRO_FONT_WEIGHT_NORMAL, 
		12.0,
		BStyles::Font::TEXT_ALIGN_LEFT, 
		BStyles::Font::TEXT_VALIGN_MIDDLE
	);

	BStyles::Theme theme = BStyles::Theme
	{
		// main
		{
			URID ("/main"),
			BStyles::Style 
			({
				{BURID(BSTYLES_STYLEPROPERTY_BACKGROUND_URI), BUtilities::makeAny<BStyles::Fill>(widgetBg)},
				{BURID(BSTYLES_STYLEPROPERTY_BORDER_URI), BUtilities::makeAny<BStyles::Border>(BStyles::noBorder)}
			})
		},

		// screen
		{
			URID ("/screen"), 
			BStyles::Style 
			({
				{BURID(BSTYLES_STYLEPROPERTY_BACKGROUND_URI), BUtilities::makeAny<BStyles::Fill>(screenBg)},
				{BURID(BSTYLES_STYLEPROPERTY_BORDER_URI), BUtilities::makeAny<BStyles::Border>(BStyles::noBorder)}
			})
		},

		// button
		{
			URID ("/button"), 
			BStyles::Style 
			({
				{BURID(BSTYLES_STYLEPROPERTY_BACKGROUND_URI), BUtilities::makeAny<BStyles::Fill>({BStyles::Fill(fgColors[BStyles::STATUS_NORMAL])})},
				{BURID(BSTYLES_STYLEPROPERTY_BGCOLORS_URI), BUtilities::makeAny<BStyles::ColorMap>(fgColors)},
				{BURID(BSTYLES_STYLEPROPERTY_BORDER_URI), BUtilities::makeAny<BStyles::Border>(btBorder)}
			})
		},
		
		// button/label
		{
			URID ("/button/label"), 
			BStyles::Style
			({	
				{BURID(BSTYLES_STYLEPROPERTY_FONT_URI), BUtilities::makeAny<BStyles::Font>(defaultFont)},
				{BURID(BSTYLES_STYLEPROPERTY_TXCOLORS_URI), BUtilities::makeAny<BStyles::ColorMap>(bgColors)}
			})
		},

		// redbutton
		{
			URID ("/redbutton"), 
			BStyles::Style 
			({
				{BURID(BSTYLES_STYLEPROPERTY_BGCOLORS_URI), BUtilities::makeAny<BStyles::ColorMap>(bgColors)},
				{BURID(BSTYLES_STYLEPROPERTY_FGCOLORS_URI), BUtilities::makeAny<BStyles::ColorMap>(BStyles::reds)}
			})
		},

		// halobutton
		{
			URID ("/halobutton"), 
			BStyles::Style 
			({
				{BURID(BSTYLES_STYLEPROPERTY_BACKGROUND_URI), BUtilities::makeAny<BStyles::Fill>({BStyles::noFill})},
				{BURID(BSTYLES_STYLEPROPERTY_BORDER_URI), BUtilities::makeAny<BStyles::Border>(BStyles::noBorder)}
			})
		},

		// label
		{
			URID ("/label"), 
			BStyles::Style
			({	
				{BURID(BSTYLES_STYLEPROPERTY_FONT_URI), BUtilities::makeAny<BStyles::Font>(defaultFont)},
				{BURID(BSTYLES_STYLEPROPERTY_TXCOLORS_URI), BUtilities::makeAny<BStyles::ColorMap>(txColors)}
			})
		},

		// rlabel
		{
			URID ("/rlabel"), 
			BStyles::Style
			({	
				{BURID(BSTYLES_STYLEPROPERTY_FONT_URI), BUtilities::makeAny<BStyles::Font>(rFont)},
				{BURID(BSTYLES_STYLEPROPERTY_TXCOLORS_URI), BUtilities::makeAny<BStyles::ColorMap>(txColors)}
			})
		},

		// dot
		{
			URID ("/dot"), 
			BStyles::Style
			({	
				{BURID(BSTYLES_STYLEPROPERTY_FGCOLORS_URI), BUtilities::makeAny<BStyles::ColorMap>(txColors)}
			})
		},

		// dial
		{
			URID ("/dial"), 
			BStyles::Style
			({	
				{BURID(BSTYLES_STYLEPROPERTY_BGCOLORS_URI), BUtilities::makeAny<BStyles::ColorMap>(BStyles::darks)},
				{BURID(BSTYLES_STYLEPROPERTY_FGCOLORS_URI), BUtilities::makeAny<BStyles::ColorMap>(fgColors)},
				{URID ("/dial/label"), BUtilities::makeAny<BStyles::Style>
					(
						BStyles::Style
						{
							{BURID(BSTYLES_STYLEPROPERTY_FONT_URI), BUtilities::makeAny<BStyles::Font>(smFont)},
							{BURID(BSTYLES_STYLEPROPERTY_TXCOLORS_URI), BUtilities::makeAny<BStyles::ColorMap>(txColors)}
						}
					)}
			})
		}

	};
};

#endif /* BANGRGUI_HPP_ */