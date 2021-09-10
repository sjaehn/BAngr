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

#include "BAngrGUI.hpp"
#include <exception>
#include "screen.h"
#include "BUtilities/vsystem.hpp"


BAngrGUI::BAngrGUI (const char *bundle_path, const LV2_Feature *const *features, PuglNativeView parentWindow) :
	Window (1000, 560, "B.Angr", parentWindow, true, PUGL_MODULE, 0),
	controller (NULL), 
	write_function (NULL),
	pluginPath (bundle_path ? std::string (bundle_path) : std::string ("")),  
	sz (1.0), 
	bgImageSurface (nullptr),
	map (nullptr),

	mContainer (0, 0, 1000, 560, "main"),
	cursor (480, 260, 40, 40, "dot"),
	poweredLabel (720, 540, 250, 20, "rlabel", BANGR_LABEL_POWERED_BY),
	helpButton (918, 508, 24, 24, "widget", BANGR_LABEL_HELP),
	ytButton (948, 508, 24, 24, "widget", BANGR_LABEL_VIDEO),
	bypassButton (900, 30, 20, 20, "dial"),
	bypassLabel (880, 60, 60, 20, "label", BANGR_LABEL_BYPASS),
	drywetDial (940, 20, 40, 40, "dial", 1.0, 0.0, 1.0, 0.0, ""),
	drywetLabel (930, 60, 60, 20, "label", BANGR_LABEL_DRY_WET),
	speedDial (370, 460, 60, 60, "dial", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", ""),
	speedLabel (370, 520, 60, 20, "label", BANGR_LABEL_SPEED),
	spinDial (570, 460, 60, 60, "dial", 0.0, -1.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", ""),
	spinLabel (570, 520, 60, 20, "label", BANGR_LABEL_SPIN),
	speedScreen (180, 480, 100, 35, "screen"),
	speedFlexLabel (220, 520, 100, 20, "label", BANGR_LABEL_FLEXIBILITY),
	speedTypeListbox 
	(
		280, 490, 80, 20, 0, -120, 80, 120, "menu", 
		BItems::ItemList 
		({
			{RANDOM, BANGR_LABEL_RANDOM}, 
			{LEVEL, BANGR_LABEL_LEVEL},
			{LOWS, BANGR_LABEL_LOWS},
			{MIDS, BANGR_LABEL_MIDS},
			{HIGHS, BANGR_LABEL_HIGHS}
		}), 
		0),
	speedAmountSlider (200, 485, 80, 20, "slider", 0.0, 0.0, 1.0, 0.0, "%1.2f"),
	spinScreen (640, 480, 100, 35, "screen"),
	spinFlexLabel (680, 520, 100, 20, "label", BANGR_LABEL_FLEXIBILITY),
	spinTypeListbox 
	(
		740, 490, 80, 20, 0, -120, 80, 120, "menu",
		BItems::ItemList 
		({
			{RANDOM, BANGR_LABEL_RANDOM}, 
			{LEVEL, BANGR_LABEL_LEVEL},
			{LOWS, BANGR_LABEL_LOWS},
			{MIDS, BANGR_LABEL_MIDS},
			{HIGHS, BANGR_LABEL_HIGHS}
		}), 
		0),
	spinAmountSlider (660, 485, 80, 20, "slider", 0.0, 0.0, 1.0, 0.0, "%1.2f")
{
	// Init param widgets
	for (int i = 0; i < NR_FX; ++i)
	{
		fx[i].container = BWidgets::Widget (20 + int (i / 2) * 660, 100 + int (((i + 1) & 3) / 2) * 200, 300, 160, "widget");
		fx[i].paramDials[0] = Dial (0, 10, 60, 60, "dial", 0.5, 0.0, 1.0, 0.0, "%1.2f");
		fx[i].paramLabels[0] = BWidgets::Label (0, 70, 60, 20, "label", BANGR_LABEL_GAIN);
		fx[i].paramDials[1] = Dial (80, 10, 60, 60, "dial", 0.5, 0.0, 1.0, 0.0, "%1.2f");
		fx[i].paramLabels[1] = BWidgets::Label (80, 70, 60, 20, "label", BANGR_LABEL_FIRST);
		fx[i].paramDials[2] = Dial (160, 10, 60, 60, "dial", 0.5, 0.0, 1.0, 0.0, "%1.2f");
		fx[i].paramLabels[2] = BWidgets::Label (160, 70, 60, 20, "label", BANGR_LABEL_LAST);
		fx[i].paramDials[3] = Dial (240, 10, 60, 60, "dial", 0.0, 0.0, 1.0, 0.0, "%1.2f");
		fx[i].paramLabels[3] = BWidgets::Label (240, 70, 60, 20, "label", BANGR_LABEL_NUKE);
		fx[i].paramDials[4] = Dial (40, 70, 60, 60, "dial", 1.0, 0.0, 1.0, 0.0, "%1.2f");
		fx[i].paramLabels[4] = BWidgets::Label (40, 130, 60, 20, "label", BANGR_LABEL_MIX);
		fx[i].paramDials[5] = Dial (200, 70, 60, 60, "dial", 0.0, -1.0, 1.0, 0.0, "%1.2f");
		fx[i].paramLabels[5] = BWidgets::Label (200, 130, 60, 20, "label", BANGR_LABEL_PAN);
	}

	// Link controllers
	controllerWidgets[BYPASS] = &bypassButton;
	controllerWidgets[DRY_WET] = &drywetDial;
	controllerWidgets[SPEED] = &speedDial;
	controllerWidgets[SPEED_RANGE] = &speedDial.range;
	controllerWidgets[SPEED_TYPE] = &speedTypeListbox;
	controllerWidgets[SPEED_AMOUNT] = &speedAmountSlider;
	controllerWidgets[SPIN] = &spinDial;
	controllerWidgets[SPIN_RANGE] = &spinDial.range;
	controllerWidgets[SPIN_TYPE] = &spinTypeListbox;
	controllerWidgets[SPIN_AMOUNT] = &spinAmountSlider;

	for (int i = 0; i < NR_FX; ++i)
	{
		for (int j = 0; j < NR_PARAMS; ++j)
		{
			controllerWidgets[FX + i * NR_PARAMS + j] = &fx[i].paramDials[j];
		}
	}

	// Configure widgets
	cursor.setDraggable (true);


	// Set callbacks
	for (BWidgets::ValueWidget* c : controllerWidgets) c->setCallbackFunction (BEvents::EventType::VALUE_CHANGED_EVENT, BAngrGUI::valueChangedCallback);
	cursor.setCallbackFunction (BEvents::EventType::POINTER_DRAG_EVENT, BAngrGUI::cursorDraggedCallback);
	cursor.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, BAngrGUI::cursorDraggedCallback);
	cursor.setCallbackFunction (BEvents::EventType::BUTTON_RELEASE_EVENT, BAngrGUI::cursorReleasedCallback);
	poweredLabel.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, BAngrGUI::xregionClickedCallback);
	helpButton.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, BAngrGUI::helpButtonClickedCallback);
	ytButton.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, BAngrGUI::ytButtonClickedCallback);

	// Load background & apply theme
	bgImageSurface = cairo_image_surface_create_from_png ((pluginPath + BG_FILE).c_str());
	widgetBg.loadFillFromCairoSurface (bgImageSurface);
	applyTheme (theme);

	// Pack widgets
	for (Fx& f : fx)
	{
		for (Dial& d : f.paramDials) f.container.add (d);
		for (BWidgets::Label& l : f.paramLabels) f.container.add (l);
		mContainer.add (f.container);
	}
	mContainer.add (bypassButton);
	mContainer.add (bypassLabel);
	mContainer.add (drywetDial);
	mContainer.add (drywetLabel);
	mContainer.add (speedDial);
	mContainer.add (speedLabel);
	mContainer.add (spinDial);
	mContainer.add (spinLabel);
	mContainer.add (speedFlexLabel);
	mContainer.add (speedTypeListbox);
	mContainer.add (speedAmountSlider);
	mContainer.add (spinFlexLabel);
	mContainer.add (spinTypeListbox);
	mContainer.add (spinAmountSlider);
	mContainer.add (speedScreen);
	mContainer.add (spinScreen);
	mContainer.add (cursor);
	mContainer.add (poweredLabel);
	mContainer.add (helpButton);
	mContainer.add (ytButton);
	add (mContainer);

	//Scan host features for URID map
	LV2_URID_Map* m = NULL;
	// unmap = NULL;

	for (int i = 0; features[i]; ++i)
	{
		if (strcmp(features[i]->URI, LV2_URID__map) == 0) m = (LV2_URID_Map*) features[i]->data;
		// if (strcmp(features[i]->URI, LV2_URID__unmap) == 0) unmap = (LV2_URID_Unmap*) features[i]->data;
	}
	if ((!m) /*|| (!unmap)*/ ) throw std::invalid_argument ("Host does not support urid:map");

	//Map URIS
	map = m;
	getURIs (map, &urids);

	// Initialize forge
	lv2_atom_forge_init (&forge,map);
}

BAngrGUI::~BAngrGUI()
{}

void BAngrGUI::portEvent(uint32_t port_index, uint32_t buffer_size, uint32_t format, const void* buffer)
{
	// Notify port
	if ((format == urids.atom_eventTransfer) && (port_index == NOTIFY))
	{
		const LV2_Atom* atom = (const LV2_Atom*) buffer;
		if (lv2_atom_forge_is_object_type(&forge, atom->type))
		{
			const LV2_Atom_Object* obj = (const LV2_Atom_Object*) atom;
			
			if (obj->body.otype == urids.patch_Set)
			{
				const LV2_Atom* property = NULL;
      			const LV2_Atom* value    = NULL;
				lv2_atom_object_get
				(
					obj,
                    urids.patch_property, &property,
                    urids.patch_value, &value,
                    NULL
				);

				if (property && (property->type == urids.atom_URID) && value)
				{
					const uint32_t key = ((const LV2_Atom_URID*)property)->body;
					
					if ((key == urids.bangr_xcursor) && (value->type == urids.atom_Float)) 
					{
						const float xcursor = ((LV2_Atom_Float*)value)->body;
						cursor.moveTo ((400.0 + xcursor * 200.0) * sz - 0.5 * cursor.getWidth(), cursor.getPosition().y);
					}

					else if ((key == urids.bangr_ycursor) && (value->type == urids.atom_Float)) 
					{
						const float ycursor = ((LV2_Atom_Float*)value)->body;
						cursor.moveTo (cursor.getPosition().x, (180.0 + ycursor * 200.0) * sz - 0.5 * cursor.getHeight());
					}
				}
			}
		}
	}

	// Scan controller ports
	else if ((format == 0) && (port_index >= CONTROLLERS) && (port_index < CONTROLLERS + NR_CONTROLLERS))
	{
		float* pval = (float*) buffer;
		controllerWidgets[port_index - CONTROLLERS]->setValue (*pval);
	}
}

void BAngrGUI::resizeGUI()
{
	hide ();

	// Resize Fonts
	defaultFont.setFontSize (12 * sz);
	rFont.setFontSize (12 * sz);
	lFont.setFontSize (12 * sz);

	// Resize Background
	cairo_surface_t* surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 1000 * sz, 560 * sz);
	cairo_t* cr = cairo_create (surface);
	cairo_scale (cr, sz, sz);
	cairo_set_source_surface(cr, bgImageSurface, 0, 0);
	cairo_paint(cr);
	widgetBg.loadFillFromCairoSurface(surface);
	cairo_destroy (cr);
	cairo_surface_destroy (surface);

	// Resize widgets
	RESIZE (mContainer, 0, 0, 1000, 560, sz);
	cursor.resize (40.0 * sz, 40.0 * sz);
	RESIZE (poweredLabel, 720, 540, 250, 20, sz);
	RESIZE (helpButton, 918, 508, 24, 24, sz);
	RESIZE (ytButton, 948, 508, 24, 24, sz);
	RESIZE (bypassButton, 900, 30, 20, 20, sz);
	RESIZE (bypassLabel, 880, 60, 60, 20, sz);
	RESIZE (drywetDial, 940, 20, 40, 40, sz);
	RESIZE (drywetLabel, 930, 60, 60, 20, sz);
	RESIZE (speedDial, 370, 460, 60, 60, sz);
	RESIZE (speedLabel, 370, 520, 60, 20, sz);
	RESIZE (spinDial, 570, 460, 60, 60, sz);
	RESIZE (spinLabel, 570, 520, 60, 20, sz);
	RESIZE (speedScreen, 180, 480, 100, 35, sz);
	RESIZE (speedFlexLabel, 220, 520, 100, 20, sz);
	RESIZE (speedTypeListbox, 280, 490, 80, 20, sz);
	speedTypeListbox.resizeListBox(BUtilities::Point (80 * sz, 120 * sz));
	speedTypeListbox.moveListBox(BUtilities::Point (0, -120 * sz));
	speedTypeListbox.resizeListBoxItems(BUtilities::Point (80 * sz, 20 * sz));
	RESIZE (speedAmountSlider, 200, 485, 80, 20, sz);
	RESIZE (spinScreen , 640, 480, 100, 35, sz);
	RESIZE (spinFlexLabel, 680, 520, 100, 20, sz);
	RESIZE (spinTypeListbox, 740, 490, 80, 20, sz); 
	spinTypeListbox.resizeListBox(BUtilities::Point (80 * sz, 120 * sz));
	spinTypeListbox.moveListBox(BUtilities::Point (0, -120 * sz));
	spinTypeListbox.resizeListBoxItems(BUtilities::Point (80 * sz, 20 * sz));
	RESIZE (spinAmountSlider, 660, 485, 80, 20, sz);

	for (int i = 0; i < NR_FX; ++i)
	{
		RESIZE (fx[i].container, 20 + int (i / 2) * 660, 100 + int (((i + 1) & 3) / 2) * 200, 300, 160, sz);
		RESIZE (fx[i].paramDials[0], 0, 10, 60, 60, sz);
		RESIZE (fx[i].paramLabels[0], 0, 70, 60, 20, sz);
		RESIZE (fx[i].paramDials[1], 80, 10, 60, 60, sz);
		RESIZE (fx[i].paramLabels[1], 80, 70, 60, 20, sz);
		RESIZE (fx[i].paramDials[2], 160, 10, 60, 60, sz);
		RESIZE (fx[i].paramLabels[2], 160, 70, 60, 20, sz);
		RESIZE (fx[i].paramDials[3], 240, 10, 60, 60, sz);
		RESIZE (fx[i].paramLabels[3], 240, 70, 60, 20, sz);
		RESIZE (fx[i].paramDials[4], 40, 70, 60, 60, sz);
		RESIZE (fx[i].paramLabels[4], 40, 130, 60, 20, sz);
		RESIZE (fx[i].paramDials[5], 200, 70, 60, 60, sz);
		RESIZE (fx[i].paramLabels[5], 200, 130, 60, 20, sz);
	}

	// Apply changes
	applyTheme (theme);
	show ();
}

void BAngrGUI::applyTheme (BStyles::Theme& theme)
{
	mContainer.applyTheme (theme);
	cursor.applyTheme (theme);
	poweredLabel.applyTheme (theme);
	helpButton.applyTheme (theme);
	ytButton.applyTheme (theme);
	bypassButton.applyTheme (theme);
	bypassLabel.applyTheme (theme);
	drywetDial.applyTheme (theme);
	drywetLabel.applyTheme (theme);
	speedDial.applyTheme (theme);
	speedLabel.applyTheme (theme);
	spinDial.applyTheme (theme);
	spinLabel.applyTheme (theme);
	speedScreen.applyTheme (theme);
	speedFlexLabel.applyTheme (theme);
	speedTypeListbox.applyTheme (theme);
	speedAmountSlider.applyTheme (theme);
	spinScreen.applyTheme (theme);
	spinFlexLabel.applyTheme (theme);
	spinTypeListbox.applyTheme (theme);
	spinAmountSlider.applyTheme (theme);

	for (Fx& f : fx)
	{
		for (Dial& d : f.paramDials) d.applyTheme (theme);
		for (BWidgets::Label& l : f.paramLabels) l.applyTheme (theme);
	}
}

void BAngrGUI::onConfigureRequest (BEvents::ExposeEvent* event)
{
	Window::onConfigureRequest (event);

	sz = (getWidth() / 1000 > getHeight() / 560 ? getHeight() / 560 : getWidth() / 1000);
	resizeGUI ();
}

void BAngrGUI::sendCursor ()
{
	sendCursorOn();
	sendXCursor();
	sendYCursor();
}

void BAngrGUI::sendXCursor ()
{
	uint8_t obj_buf[128];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.patch_Set);
	lv2_atom_forge_key(&forge, urids.patch_property);
	lv2_atom_forge_urid(&forge, urids.bangr_xcursor);
	lv2_atom_forge_key(&forge, urids.patch_value);
	lv2_atom_forge_float(&forge, ((cursor.getPosition().x + 0.5 * cursor.getWidth()) / sz - 400.0) / 200.0);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BAngrGUI::sendYCursor ()
{
	uint8_t obj_buf[128];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.patch_Set);
	lv2_atom_forge_key(&forge, urids.patch_property);
	lv2_atom_forge_urid(&forge, urids.bangr_ycursor);
	lv2_atom_forge_key(&forge, urids.patch_value);
	lv2_atom_forge_float(&forge, ((cursor.getPosition().y + 0.5 * cursor.getHeight()) / sz - 180.0) / 200.0);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BAngrGUI::sendCursorOn ()
{
	uint8_t obj_buf[64];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.bangr_cursorOn);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BAngrGUI::sendCursorOff ()
{
	uint8_t obj_buf[64];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.bangr_cursorOff);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BAngrGUI::valueChangedCallback (BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ValueWidget* widget = (BWidgets::ValueWidget*) event->getWidget ();
	if (!widget) return;
	float value = widget->getValue();
	BAngrGUI* ui = (BAngrGUI*) widget->getMainWindow();
	if (!ui) return;

	int controllerNr = -1;

	// Identify controller
	for (int i = 0; i < NR_CONTROLLERS; ++i)
	{
		if (widget == ui->controllerWidgets[i])
		{
			controllerNr = i;
			break;
		}
	}

	// Controllers
	if (controllerNr >= 0)
	{
		if (controllerNr == SPEED_TYPE)
		{
			if (value == RANDOM) ui->speedScreen.show();
			else ui->speedScreen.hide();
		}

		if (controllerNr == SPIN_TYPE)
		{
			if (value == RANDOM) ui->spinScreen.show();
			else ui->spinScreen.hide();
		}

		ui->write_function(ui->controller, CONTROLLERS + controllerNr, sizeof(float), 0, &value);
	}
}

void BAngrGUI::cursorDraggedCallback (BEvents::Event* event)
{
	if (!event) return;
	BEvents::PointerEvent* pev = (BEvents::PointerEvent*)event;
	Dot* widget = (Dot*) event->getWidget ();
	if (!widget) return;
	BAngrGUI* ui = (BAngrGUI*) widget->getMainWindow();
	if (!ui) return;
	if (widget != &ui->cursor) return;

	double x = ui->cursor.getPosition().x + 0.5 * ui->cursor.getWidth() + pev->getDelta().x;
	double y = ui->cursor.getPosition().y + 0.5 * ui->cursor.getHeight() + pev->getDelta().y;

	if (x < 400 * ui->sz) x = 400 * ui->sz;
	if (x > 600 * ui->sz) x = 600 * ui->sz;
	if (y < 180 * ui->sz) y = 180 * ui->sz;
	if (y > 380 * ui->sz) y = 380 * ui->sz;

	ui->cursor.moveTo (x - 0.5 * ui->cursor.getWidth(), y - 0.5 * ui->cursor.getHeight());
	ui->sendCursor();
}

void BAngrGUI::cursorReleasedCallback (BEvents::Event* event)
{
	if (!event) return;
	//BEvents::PointerEvent* pev = (BEvents::PointerEvent*)event;
	Dot* widget = (Dot*) event->getWidget ();
	if (!widget) return;
	BAngrGUI* ui = (BAngrGUI*) widget->getMainWindow();
	if (!ui) return;
	if (widget != &ui->cursor) return;

	ui->sendCursorOff();
}

void BAngrGUI::xregionClickedCallback (BEvents::Event* event)
{
	char cmd[] = WWW_BROWSER_CMD;
	char param[] = XREGION_URL;
	char* argv[] = {cmd, param, NULL};
	std::cerr << "BAngr.lv2#GUI: Call " << XREGION_URL << " for Airwindows XRegion.\n";
	if (BUtilities::vsystem (argv) == -1) std::cerr << "BAngr.lv2#GUI: Couldn't fork.\n";
}

void BAngrGUI::helpButtonClickedCallback (BEvents::Event* event)
{
	char cmd[] = WWW_BROWSER_CMD;
	char param[] = HELP_URL;
	char* argv[] = {cmd, param, NULL};
	std::cerr << "BAngr.lv2#GUI: Call " << HELP_URL << " for help.\n";
	if (BUtilities::vsystem (argv) == -1) std::cerr << "BAngr.lv2#GUI: Couldn't fork.\n";
}

void BAngrGUI::ytButtonClickedCallback (BEvents::Event* event)
{
	char cmd[] = WWW_BROWSER_CMD;
	char param[] = YT_URL;
	char* argv[] = {cmd, param, NULL};
	std::cerr << "BAngr.lv2#GUI: Call " << YT_URL << " for preview video.\n";
	if (BUtilities::vsystem (argv) == -1) std::cerr << "BAngr.lv2#GUI: Couldn't fork.\n";
}

static LV2UI_Handle instantiate (const LV2UI_Descriptor *descriptor, const char *plugin_uri, const char *bundle_path,
						  LV2UI_Write_Function write_function, LV2UI_Controller controller, LV2UI_Widget *widget,
						  const LV2_Feature *const *features)
{
	PuglNativeView parentWindow = 0;
	LV2UI_Resize* resize = NULL;

	if (strcmp(plugin_uri, BANGR_URI) != 0)
	{
		std::cerr << "BAngr.lv2#GUI: GUI does not support plugin with URI " << plugin_uri << std::endl;
		return NULL;
	}

	for (int i = 0; features[i]; ++i)
	{
		if (!strcmp(features[i]->URI, LV2_UI__parent)) parentWindow = (PuglNativeView) features[i]->data;
		else if (!strcmp(features[i]->URI, LV2_UI__resize)) resize = (LV2UI_Resize*)features[i]->data;
	}
	if (parentWindow == 0) std::cerr << "BAngr.lv2#GUI: No parent window.\n";

	// New instance
	BAngrGUI* ui;
	try {ui = new BAngrGUI (bundle_path, features, parentWindow);}
	catch (std::exception& exc)
	{
		std::cerr << "BAngr.lv2#GUI: Instantiation failed. " << exc.what () << std::endl;
		return NULL;
	}

	ui->controller = controller;
	ui->write_function = write_function;

	// Reduce min GUI size for small displays
	double sz = 1.0;
	int screenWidth  = getScreenWidth ();
	int screenHeight = getScreenHeight ();
	if ((screenWidth < 1040) || (screenHeight < 600)) sz = 0.66;

	/*
	std::cerr << "BAngrGUI.lv2 screen size " << screenWidth << " x " << screenHeight <<
			". Set GUI size to " << 800 * sz << " x " << 560 * sz << ".\n";
	*/

	if (resize) resize->ui_resize(resize->handle, 1000 * sz, 560 * sz);

	*widget = (LV2UI_Widget) puglGetNativeWindow (ui->getPuglView ());
	return (LV2UI_Handle) ui;
}

static void cleanup(LV2UI_Handle ui)
{
	BAngrGUI* pluginGui = (BAngrGUI*) ui;
	if (pluginGui) delete pluginGui;
}

static void portEvent(LV2UI_Handle ui, uint32_t port_index, uint32_t buffer_size,
	uint32_t format, const void* buffer)
{
	BAngrGUI* pluginGui = (BAngrGUI*) ui;
	if (pluginGui) pluginGui->portEvent(port_index, buffer_size, format, buffer);
}

static int callIdle (LV2UI_Handle ui)
{
	BAngrGUI* pluginGui = (BAngrGUI*) ui;
	if (pluginGui) pluginGui->handleEvents ();
	return 0;
}

static int callResize (LV2UI_Handle ui, int width, int height)
{
	BAngrGUI* self = (BAngrGUI*) ui;
	if (!self) return 0;

	BEvents::ExposeEvent* ev = new BEvents::ExposeEvent (self, self, BEvents::CONFIGURE_REQUEST_EVENT, self->getPosition().x, self->getPosition().y, width, height);
	self->addEventToQueue (ev);
	return 0;
}

static const LV2UI_Idle_Interface idle = {callIdle};
static const LV2UI_Resize resize = {nullptr, callResize};

static const void* extensionData(const char* uri)
{
	if (!strcmp(uri, LV2_UI__idleInterface)) return &idle;
	else if(!strcmp(uri, LV2_UI__resize)) return &resize;
	else return NULL;
}

static const LV2UI_Descriptor guiDescriptor = 
{
		BANGR_GUI_URI,
		instantiate,
		cleanup,
		portEvent,
		extensionData
};

// LV2 Symbol Export
LV2_SYMBOL_EXPORT const LV2UI_Descriptor *lv2ui_descriptor(uint32_t index)
{
	switch (index) {
	case 0: return &guiDescriptor;
	default:return NULL;
    }
}
