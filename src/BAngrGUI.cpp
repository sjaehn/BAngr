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

#include "BAngrGUI.hpp"
#include <cmath>
#include <exception>
#include "BAngrDial.hpp"
#include "BWidgets/BEvents/ExposeEvent.hpp"
#include "BWidgets/BUtilities/Dictionary.hpp"
#include "BWidgets/BWidgets/ComboBox.hpp"
#include "BWidgets/BWidgets/Label.hpp"
#include "BWidgets/BWidgets/Supports/ValueTransferable.hpp"
#include "BWidgets/BWidgets/Supports/ValueableTyped.hpp"
#include "Ports.hpp"
#include "screen.h"
#include "BWidgets/BUtilities/vsystem.hpp"


BAngrGUI::BAngrGUI (const char *bundle_path, const LV2_Feature *const *features, PuglNativeView parentWindow) :
	Window (1000, 560, parentWindow, URID(), "B.Choppr", true, PUGL_MODULE, 0),
	controller (NULL), 
	write_function (NULL),
	pluginPath (bundle_path ? std::string (bundle_path) : std::string ("")),
	map (nullptr),

	mContainer(0, 0, 1000, 560, pluginPath + "inc/surface.png", URID ("/bgimage")),
	cursor (480, 260, 40, 40, URID ("/dot")),
	poweredLabel (720, 540, 250, 20, BDICT("Powered by Airwindows XRegion"), URID ("/rlabel")),
	helpButton (918, 508, 24, 24, false, false, URID ("/halobutton"), BDICT ("Help")),
	ytButton (948, 508, 24, 24, false, false, URID ("/halobutton"), BDICT ("Preview")),
	bypassButton (900, 30, 20, 20, 2, true, false, URID ("/redbutton"), BDICT ("Bypass")),
	bypassLabel (880, 60, 60, 20, BDICT ("Bypass"), URID ("/label")),
	drywetDial (940, 20, 40, 40, 1.0, 0.0, 1.0, 0.0, BNOTRANSFERD, BNOTRANSFERD, BDOUBLE_TO_STRING, BSTRING_TO_DOUBLE, URID ("/dial"), BDICT ("Dry/wet")),
	drywetLabel (930, 60, 60, 20, BDICT ("Dry/wet"), URID ("/label")),
	speedDial (370, 460, 60, 60, 0.5, 0.5, 0.0, 1.0, 0.0, BNOTRANSFERD, BNOTRANSFERD, BDOUBLE_TO_STRING, BSTRING_TO_DOUBLE, URID ("/dial"), BDICT ("Speed")),
	speedLabel (370, 520, 60, 20, BDICT("Speed"), URID("/label")),
	spinDial (570, 460, 60, 60, 0.0, 0.0, -1.0, 1.0, 0.0, BNOTRANSFERD, BNOTRANSFERD, BDOUBLE_TO_STRING, BSTRING_TO_DOUBLE, URID ("/dial"), BDICT ("Spin")),
	spinLabel (570, 520, 60, 20, BDICT("Spin"), URID("/label")),
	speedScreen (180, 480, 100, 35, URID("/screen")),
	speedFlexLabel (220, 520, 100, 20, BDICT("Flexibility"), URID("/label")),
	speedTypeCombobox (280, 490, 80, 20, 0, -120, 80, 120, {BDICT("Random"), BDICT("Level"), BDICT("Lows"), BDICT("Mids"), BDICT("Highs")}, 1, URID("/menu")),
	speedAmountSlider (200, 485, 80, 20, 0.0, 0.0, 1.0, 0.0, BNOTRANSFERD, BNOTRANSFERD, BDOUBLE_TO_STRING, BSTRING_TO_DOUBLE, URID ("/dial"), BDICT ("Speed")),
	spinScreen (640, 480, 100, 35, URID("/screen")),
	spinFlexLabel (680, 520, 100, 20, BDICT("Flexibility"), URID("/label")),
	spinTypeCombobox (740, 490, 80, 20, 0, -120, 80, 120, {BDICT("Random"), BDICT("Level"), BDICT("Lows"), BDICT("Mids"), BDICT("Highs")}, 1, URID("/menu")),
	spinAmountSlider (660, 485, 80, 20, 0.0, 0.0, 1.0, 0.0, BNOTRANSFERD, BNOTRANSFERD, BDOUBLE_TO_STRING, BSTRING_TO_DOUBLE, URID ("/dial"), BDICT ("Speed"))
{
	// Init param widgets
	for (int i = 0; i < NR_FX; ++i)
	{
		fx[i].container = new BWidgets::Widget (20 + int (i / 2) * 660, 100 + int (((i + 1) & 3) / 2) * 200, 300, 160, URID ("/widget"));
		fx[i].paramDials[0] = new BAngrDial (0, 10, 60, 60, 0.5, 0.0, 1.0, 0.0, BNOTRANSFERD, BNOTRANSFERD, BDOUBLE_TO_STRING, BSTRING_TO_DOUBLE, URID ("/dial"), BDICT ("Gain"));
		fx[i].paramLabels[0] = new BWidgets::Label (0, 70, 60, 20, BDICT("Gain"), URID("/label"));
		fx[i].paramDials[1] = new BAngrDial (80, 10, 60, 60, 0.5, 0.0, 1.0, 0.0, BNOTRANSFERD, BNOTRANSFERD, BDOUBLE_TO_STRING, BSTRING_TO_DOUBLE, URID ("/dial"), BDICT ("First"));
		fx[i].paramLabels[1] = new BWidgets::Label (80, 70, 60, 20, BDICT("First"), URID("/label"));
		fx[i].paramDials[2] = new BAngrDial (160, 10, 60, 60, 0.5, 0.0, 1.0, 0.0, BNOTRANSFERD, BNOTRANSFERD, BDOUBLE_TO_STRING, BSTRING_TO_DOUBLE, URID ("/dial"), BDICT ("Last"));
		fx[i].paramLabels[2] = new BWidgets::Label (160, 70, 60, 20, BDICT("Last"), URID("/label"));
		fx[i].paramDials[3] = new BAngrDial (240, 10, 60, 60, 0.0, 0.0, 1.0, 0.0, BNOTRANSFERD, BNOTRANSFERD, BDOUBLE_TO_STRING, BSTRING_TO_DOUBLE, URID ("/dial"), BDICT ("Nuke"));
		fx[i].paramLabels[3] = new BWidgets::Label (240, 70, 60, 20, BDICT("Nuke"), URID("/label"));
		fx[i].paramDials[4] = new BAngrDial (40, 70, 60, 60, 1.0, 0.0, 1.0, 0.0, BNOTRANSFERD, BNOTRANSFERD, BDOUBLE_TO_STRING, BSTRING_TO_DOUBLE, URID ("/dial"), BDICT ("Mix"));
		fx[i].paramLabels[4] = new BWidgets::Label (40, 130, 60, 20, BDICT("Mix"), URID("/label"));
		fx[i].paramDials[5] = new BAngrDial (200, 70, 60, 60, 0.0, -1.0, 1.0, 0.0, BNOTRANSFERD, BNOTRANSFERD, BDOUBLE_TO_STRING, BSTRING_TO_DOUBLE, URID ("/dial"), BDICT ("Pan"));
		fx[i].paramLabels[5] = new BWidgets::Label (200, 130, 60, 20, BDICT("Pan"), URID("/label"));
	}

	// Link controllers
	controllerWidgets[BYPASS] = &bypassButton;
	controllerWidgets[DRY_WET] = &drywetDial;
	controllerWidgets[SPEED] = &speedDial;
	controllerWidgets[SPEED_RANGE] = &speedDial.range;
	controllerWidgets[SPEED_TYPE] = &speedTypeCombobox;
	controllerWidgets[SPEED_AMOUNT] = &speedAmountSlider;
	controllerWidgets[SPIN] = &spinDial;
	controllerWidgets[SPIN_RANGE] = &spinDial.range;
	controllerWidgets[SPIN_TYPE] = &spinTypeCombobox;
	controllerWidgets[SPIN_AMOUNT] = &spinAmountSlider;

	for (int i = 0; i < NR_FX; ++i)
	{
		for (int j = 0; j < NR_PARAMS; ++j)
		{
			controllerWidgets[FX + i * NR_PARAMS + j] = fx[i].paramDials[j];
		}
	}

	// Configure widgets
	cursor.setDraggable (true);
	drywetDial.setClickable(false);
	drywetDial.setScrollable(true);
	drywetDial.label.hide();
	speedDial.setClickable(false);
	speedDial.setScrollable(true);
	spinDial.setClickable(false);
	spinDial.setScrollable(true);
	speedAmountSlider.setClickable(false);
	speedAmountSlider.setScrollable(true);
	spinAmountSlider.setClickable(false);
	spinAmountSlider.setScrollable(true);
	for (Fx& f : fx)
	{
		for (BAngrDial* b : f.paramDials)
		{
			b->setClickable(false);
			b->setScrollable(true);
		}
	}
	setTheme(theme);


	// Set callbacks
	for (BWidgets::Widget* c : controllerWidgets) c->setCallbackFunction (BEvents::Event::VALUE_CHANGED_EVENT, BAngrGUI::valueChangedCallback);
	cursor.setCallbackFunction (BEvents::Event::POINTER_DRAG_EVENT, BAngrGUI::cursorDraggedCallback);
	cursor.setCallbackFunction (BEvents::Event::BUTTON_PRESS_EVENT, BAngrGUI::cursorDraggedCallback);
	cursor.setCallbackFunction (BEvents::Event::BUTTON_RELEASE_EVENT, BAngrGUI::cursorReleasedCallback);
	poweredLabel.setCallbackFunction (BEvents::Event::BUTTON_PRESS_EVENT, BAngrGUI::xregionClickedCallback);
	helpButton.setCallbackFunction (BEvents::Event::BUTTON_PRESS_EVENT, BAngrGUI::helpButtonClickedCallback);
	ytButton.setCallbackFunction (BEvents::Event::BUTTON_PRESS_EVENT, BAngrGUI::ytButtonClickedCallback);

	// Pack widgets
	for (Fx& f : fx)
	{
		for (BAngrDial* d : f.paramDials) f.container->add (d);
		for (BWidgets::Label* l : f.paramLabels) f.container->add (l);
		mContainer.add (f.container);
	}
	mContainer.add (&bypassButton);
	mContainer.add (&bypassLabel);
	mContainer.add (&drywetDial);
	mContainer.add (&drywetLabel);
	mContainer.add (&speedDial);
	mContainer.add (&speedLabel);
	mContainer.add (&spinDial);
	mContainer.add (&spinLabel);
	mContainer.add (&speedFlexLabel);
	mContainer.add (&speedTypeCombobox);
	mContainer.add (&speedAmountSlider);
	mContainer.add (&spinFlexLabel);
	mContainer.add (&spinTypeCombobox);
	mContainer.add (&spinAmountSlider);
	mContainer.add (&speedScreen);
	mContainer.add (&spinScreen);
	mContainer.add (&cursor);
	mContainer.add (&poweredLabel);
	mContainer.add (&helpButton);
	mContainer.add (&ytButton);
	add (&mContainer);

	//Scan host features for URID map
	LV2_URID_Map* m = NULL;

	for (int i = 0; features[i]; ++i)
	{
		if (strcmp(features[i]->URI, LV2_URID__map) == 0) m = static_cast<LV2_URID_Map*> (features[i]->data);
	}
	if (!m) throw std::invalid_argument ("Host does not support urid:map");

	//Map URIS
	map = m;
	getURIs (map, &urids);

	// Initialize forge
	lv2_atom_forge_init (&forge,map);
}

BAngrGUI::~BAngrGUI()
{
	for (const Fx& fxi : fx)
	{
		delete fxi.container;
		for (const BAngrDial* d : fxi.paramDials) delete d;
		for (const BWidgets::Label* l : fxi.paramLabels) delete l;
	}
}

void BAngrGUI::portEvent(uint32_t port_index, uint32_t buffer_size, uint32_t format, const void* buffer)
{
	// Notify port
	if ((format == urids.atom_eventTransfer) && (port_index == NOTIFY))
	{
		const LV2_Atom* atom = static_cast<const LV2_Atom*> (buffer);
		if (lv2_atom_forge_is_object_type(&forge, atom->type))
		{
			const LV2_Atom_Object* obj = reinterpret_cast<const LV2_Atom_Object*> (atom);
			
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
					const uint32_t key = reinterpret_cast<const LV2_Atom_URID*>(property)->body;
					
					if ((key == urids.bangr_xcursor) && (value->type == urids.atom_Float)) 
					{
						const float xcursor = reinterpret_cast<const LV2_Atom_Float*>(value)->body;
						cursor.moveTo ((400.0 + xcursor * 200.0) - 0.5 * cursor.getWidth(), cursor.getPosition().y);
					}

					else if ((key == urids.bangr_ycursor) && (value->type == urids.atom_Float)) 
					{
						const float ycursor = reinterpret_cast<const LV2_Atom_Float*>(value)->body;
						cursor.moveTo (cursor.getPosition().x, (180.0 + ycursor * 200.0) - 0.5 * cursor.getHeight());
					}
				}
			}
		}
	}

	// Scan controller ports
	else if ((format == 0) && (port_index >= CONTROLLERS) && (port_index < CONTROLLERS + NR_CONTROLLERS))
	{
		const float* pval = static_cast<const float*> (buffer);

		// Offst Comboboxes by 1
		if ((port_index - CONTROLLERS == SPEED_TYPE) || (port_index - CONTROLLERS == SPIN_TYPE))
		{
			BWidgets::ComboBox* combobox = dynamic_cast<BWidgets::ComboBox*>(controllerWidgets[port_index - CONTROLLERS]);
			if (combobox) combobox->setValue(*pval + 1);
		}

		// All other control ports
		else
		{
			BWidgets::ValueableTyped<double>* valueable = dynamic_cast<BWidgets::ValueableTyped<double>*>(controllerWidgets[port_index - CONTROLLERS]);
			if (valueable) valueable->setValue (*pval);
		}
	}
}

void BAngrGUI::onConfigureRequest (BEvents::Event* event)
{
	Window::onConfigureRequest (event);

	BEvents::ExposeEvent* ee = dynamic_cast<BEvents::ExposeEvent*>(event);
	if (!ee) return;
	const double sz = (ee->getArea().getWidth() / 1000.0 > ee->getArea().getHeight() / 560.0 ? ee->getArea().getHeight() / 560.0 : ee->getArea().getWidth() / 1000.0);
	setZoom (sz);
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
	LV2_Atom* msg = reinterpret_cast<LV2_Atom*>(lv2_atom_forge_object(&forge, &frame, 0, urids.patch_Set));
	lv2_atom_forge_key(&forge, urids.patch_property);
	lv2_atom_forge_urid(&forge, urids.bangr_xcursor);
	lv2_atom_forge_key(&forge, urids.patch_value);
	lv2_atom_forge_float(&forge, ((cursor.getPosition().x + 0.5 * cursor.getWidth()) - 400.0) / 200.0);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BAngrGUI::sendYCursor ()
{
	uint8_t obj_buf[128];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = reinterpret_cast<LV2_Atom*>(lv2_atom_forge_object(&forge, &frame, 0, urids.patch_Set));
	lv2_atom_forge_key(&forge, urids.patch_property);
	lv2_atom_forge_urid(&forge, urids.bangr_ycursor);
	lv2_atom_forge_key(&forge, urids.patch_value);
	lv2_atom_forge_float(&forge, ((cursor.getPosition().y + 0.5 * cursor.getHeight()) - 180.0) / 200.0);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BAngrGUI::sendCursorOn ()
{
	uint8_t obj_buf[64];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = reinterpret_cast<LV2_Atom*>(lv2_atom_forge_object(&forge, &frame, 0, urids.bangr_cursorOn));
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BAngrGUI::sendCursorOff ()
{
	uint8_t obj_buf[64];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = reinterpret_cast<LV2_Atom*>(lv2_atom_forge_object(&forge, &frame, 0, urids.bangr_cursorOff));
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BAngrGUI::valueChangedCallback (BEvents::Event* event)
{
	if (!event) return;

	BWidgets::Widget* widget = event->getWidget ();
	if (!widget) return;

	BAngrGUI* ui = dynamic_cast<BAngrGUI*> (widget->getMainWindow());
	if (!ui) return;

	int controllerNr = -1;
	float value = nanf("");

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
			value = ui->speedTypeCombobox.getValue() - 1.0f;
			if (value == RANDOM) ui->speedScreen.show();
			else ui->speedScreen.hide();
		}

		else if (controllerNr == SPIN_TYPE)
		{
			value = ui->speedTypeCombobox.getValue() - 1.0f;
			if (value == RANDOM) ui->spinScreen.show();
			else ui->spinScreen.hide();
		}

		else
		{
			// Range widgets: Update parent dials as callbacks are overwritten
			if (controllerNr == SPEED_RANGE) ui->speedDial.update();
			if (controllerNr == SPIN_RANGE) ui->spinDial.update();

			BWidgets::ValueableTyped<double>* valueable = dynamic_cast<BWidgets::ValueableTyped<double>*>(widget);
			if (valueable) value = valueable->getValue();
		}

		if (!isnanf(value)) ui->write_function(ui->controller, CONTROLLERS + controllerNr, sizeof(float), 0, &value);
	}
}

void BAngrGUI::cursorDraggedCallback (BEvents::Event* event)
{
	if (!event) return;
	Dot* widget = dynamic_cast<Dot*>(event->getWidget ());
	if (!widget) return;
	BAngrGUI* ui = dynamic_cast<BAngrGUI*> (widget->getMainWindow());
	if (!ui) return;
	if (widget != &ui->cursor) return;

	double x = std::min (std::max (ui->cursor.getPosition().x + 0.5 * ui->cursor.getWidth(), 400.0), 600.0);
	double y = std::min (std::max (ui->cursor.getPosition().y + 0.5 * ui->cursor.getHeight(), 180.0), 380.0);

	ui->cursor.moveTo (x - 0.5 * ui->cursor.getWidth(), y - 0.5 * ui->cursor.getHeight());
	ui->sendCursor();
}

void BAngrGUI::cursorReleasedCallback (BEvents::Event* event)
{
	if (!event) return;
	Dot* widget = dynamic_cast<Dot*> (event->getWidget ());
	if (!widget) return;
	BAngrGUI* ui = dynamic_cast<BAngrGUI*> (widget->getMainWindow());
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
		if (!strcmp(features[i]->URI, LV2_UI__parent)) parentWindow = reinterpret_cast<PuglNativeView> (features[i]->data);
		else if (!strcmp(features[i]->URI, LV2_UI__resize)) resize = static_cast<LV2UI_Resize*>(features[i]->data);
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
	BAngrGUI* pluginGui = static_cast<BAngrGUI*> (ui);
	if (pluginGui) delete pluginGui;
}

static void portEvent(LV2UI_Handle ui, uint32_t port_index, uint32_t buffer_size,
	uint32_t format, const void* buffer)
{
	BAngrGUI* pluginGui = static_cast<BAngrGUI*> (ui);
	if (pluginGui) pluginGui->portEvent(port_index, buffer_size, format, buffer);
}

static int callIdle (LV2UI_Handle ui)
{
	BAngrGUI* pluginGui = static_cast<BAngrGUI*> (ui);
	if (pluginGui) pluginGui->handleEvents ();
	return 0;
}

static int callResize (LV2UI_Handle ui, int width, int height)
{
	BAngrGUI* self = static_cast<BAngrGUI*> (ui);
	if (!self) return 0;

	BEvents::ExposeEvent* ev = new BEvents::ExposeEvent (self, self, BEvents::Event::CONFIGURE_REQUEST_EVENT, self->getPosition().x, self->getPosition().y, width, height);
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
