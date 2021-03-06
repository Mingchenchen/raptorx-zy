// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
// $Id: displayProperties.C,v 1.101.16.4 2007-04-24 22:56:56 amoll Exp $
//

#include <BALL/VIEW/DIALOGS/displayProperties.h>
#include <BALL/VIEW/DIALOGS/modelSettingsDialog.h>
#include <BALL/VIEW/DIALOGS/coloringSettingsDialog.h>
#include <BALL/VIEW/DIALOGS/preferences.h>

#include <BALL/VIEW/KERNEL/message.h>
#include <BALL/VIEW/KERNEL/mainControl.h>
#include <BALL/VIEW/KERNEL/common.h>

#include <BALL/VIEW/WIDGETS/geometricControl.h>

#include <BALL/VIEW/MODELS/backboneModel.h>
#include <BALL/VIEW/MODELS/cartoonModel.h>
#include <BALL/VIEW/MODELS/ballAndStickModel.h>
#include <BALL/VIEW/MODELS/lineModel.h>
#include <BALL/VIEW/MODELS/surfaceModel.h>
#include <BALL/VIEW/MODELS/vanDerWaalsModel.h>
#include <BALL/VIEW/MODELS/HBondModel.h>
#include <BALL/VIEW/MODELS/forceModel.h>
#include <BALL/VIEW/MODELS/standardColorProcessor.h>
#include <BALL/VIEW/PRIMITIVES/mesh.h>

#include <QtGui/qmenubar.h>
#include <QtGui/qlabel.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qslider.h>
#include <QtGui/qradiobutton.h>

namespace BALL
{
	namespace VIEW
	{

DisplayProperties::DisplayProperties(QWidget* parent, const char* name)
	throw()
	:	QDialog(parent),
		Ui_DisplayPropertiesData(),
		ModularWidget(name),
		PreferencesEntry(),
		model_settings_(0),
		coloring_settings_(0),
		preferences_(0),
		id_(0),
		rep_(0),
		advanced_options_modified_(false),
		create_representations_for_new_molecules_(true),
		changed_selection_color_(false)
{
#ifdef BALL_VIEW_DEBUG
	Log.error() << "new DisplayProperties " << this << std::endl;
#endif
	setupUi(this);
	setObjectName(name);
	ModularWidget::registerWidget(this);

	MainControl* mc = getMainControl();
	model_information_ = &mc->getModelInformation();

	model_type_combobox->clear();
	for (Index p = 0; p < MODEL_LABEL; p++)
	{
		if (!model_information_->modelMuteableByDisplayProperties((ModelType)p)) break;
		model_type_combobox->addItem(model_information_->getModelName((VIEW::ModelType)p).c_str());
	}

	coloring_method_combobox->clear();
	for (Index p = 0; p < COLORING_UNKNOWN; p++)
	{
		if (!model_information_->coloringMuteableByDisplayProperties((ColoringMethod)p)) break;
		coloring_method_combobox->addItem(model_information_->getColoringName((VIEW::ColoringMethod)p).c_str());
	}

	createRepresentationMode();

	setColor(selection_color_label, ColorRGBA(1.0, 1.0, 0));
	setColor(custom_color_label, ColorRGBA(.0, .0, 1.0));

	setINIFileSectionName("REPRESENTATION");
	registerWidgets_();

	connect( coloring_method_combobox, SIGNAL( currentIndexChanged(int) ), this, SLOT( selectColoringMethod(int) ) );
	connect( mode_combobox, SIGNAL( activated(int) ), this, SLOT( selectMode(int) ) );
	connect( model_type_combobox, SIGNAL( currentIndexChanged(int) ), this, SLOT( selectModel(int) ) );
	connect( precision_combobox, SIGNAL( currentIndexChanged(int) ), this, SLOT( precisionBoxChanged(int) ) );
	connect( create_button, SIGNAL( clicked() ), this, SLOT( apply() ) );
	connect( close_button, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( coloring_options, SIGNAL( clicked() ), this, SLOT( coloringOptionsPressed() ) );
	connect( custom_button, SIGNAL( clicked() ), this, SLOT( editColor() ) );
	connect( edit_selection, SIGNAL( clicked() ), this, SLOT( editSelectionColor() ) );
	connect( model_options, SIGNAL( clicked() ), this, SLOT( modelOptionsPressed() ) );
	connect( precision_slider, SIGNAL( valueChanged(int) ), this, SLOT( precisionSliderChanged() ) );
	connect( transparency_slider, SIGNAL( valueChanged(int) ), this, SLOT( transparencySliderChanged() ) );
	connect( model_updates_enabled, SIGNAL( stateChanged(int) ), this, SLOT( modelUpdatesChanged() ) );
	connect( coloring_updates_enabled, SIGNAL( stateChanged(int) ), this, SLOT( coloringUpdatesChanged() ) );
	connect( modify_button, SIGNAL( clicked() ), this, SLOT( apply() ) );
}

DisplayProperties::DisplayProperties(const DisplayProperties& /*dp*/)
	throw()
	: QDialog(),
		Ui_DisplayPropertiesData(),
		ModularWidget(*this),
		PreferencesEntry()
{
}

DisplayProperties::~DisplayProperties()
	throw()
{
#ifdef BALL_VIEW_DEBUG
	Log.error() << "deleting DisplayProperties " << this << std::endl;
#endif
}


void DisplayProperties::initializeWidget(MainControl& main_control)
	throw()
{
	(main_control.initPopupMenu(MainControl::DISPLAY));

	id_ = insertMenuEntry(MainControl::DISPLAY, "D&isplay Properties", this, 
																		 SLOT(show()), Qt::CTRL+Qt::Key_I);   
	setMenuHint("Create a new representation or modify an existing one");
	setMenuHelp("displayProperties.html");
	setIcon("colorize.png", true);

	registerForHelpSystem(this, "displayProperties.html");

	selectModel(MODEL_STICK);
}


void DisplayProperties::initializePreferencesTab(Preferences &preferences)
	throw()
{
	preferences_ = &preferences;
	if (model_settings_ == 0)
	{
		model_settings_ = new ModelSettingsDialog(this, "ModelSettings");
	}
	preferences.insertEntry(model_settings_);

	if (coloring_settings_ == 0)
	{
		coloring_settings_ = new ColoringSettingsDialog(this, "ColoringSettings");
	}
	preferences.insertEntry(coloring_settings_);
}

void DisplayProperties::finalizePreferencesTab(Preferences &preferences)
	throw()
{
	if (model_settings_) 
	{
		preferences.removeEntry(model_settings_);
		model_settings_ = 0;
	}
	if (coloring_settings_) 
	{
		preferences.removeEntry(coloring_settings_);
		coloring_settings_ = 0;
	}
}


void DisplayProperties::checkMenu(MainControl& main_control)
	throw()
{
	bool busy = main_control.isBusy();

	id_->setEnabled(!main_control.compositesAreLocked());	

	if (busy)
	{
		modify_button->setEnabled(false);
		create_button->setEnabled(false);
		return;
	}

	bool has_mcs = main_control.getMolecularControlSelection().size() > 0;
	create_button->setEnabled(has_mcs);
	create_button->setDefault(has_mcs);

	GeometricControl* gc = GeometricControl::getInstance(0);
	bool has_gcs = gc != 0 && gc->getHighlightedRepresentations().size() > 0;
	if (has_gcs)
	{
		rep_ = *gc->getHighlightedRepresentations().begin();
	}
	else
	{
		rep_ = 0;
	}

	modify_button->setEnabled(rep_ != 0);
	modify_button->setDefault(rep_ != 0);

	create_button->setEnabled(has_mcs);
	create_button->setDefault(has_mcs);
}


void DisplayProperties::show()
{
	checkDrawingPrecision_();
	QDialog::show();
	raise();
}

void DisplayProperties::createRepresentationMode()
{
	rep_ = 0;
 	setWindowTitle("Create a Representation");
	if (id_ != 0) checkMenu(*getMainControl());

	model_updates_enabled->setChecked(true);
	coloring_updates_enabled->setChecked(true);
}

void DisplayProperties::modifyRepresentationMode(Representation* rep)
{
	rep_ = rep;
 	setWindowTitle("Modify a Representation");
	if (id_ != 0) checkMenu(*getMainControl());

	if (rep_ == 0 || 
			!model_information_->modelMuteableByDisplayProperties(rep->getModelType()) ||
			getMainControl()->isBusy())
	{
		return;
	}
	
	if (model_information_->coloringMuteableByDisplayProperties(rep_->getColoringMethod()))
	{
		coloring_method_combobox->setCurrentIndex(rep_->getColoringMethod());
	}
	
	precision_combobox->setCurrentIndex(rep_->getDrawingPrecision());
	
	if (rep_->getModelType() != MODEL_UNKNOWN)
	{
		model_type_combobox->setCurrentIndex(rep_->getModelType());
	}
	mode_combobox->setCurrentIndex(rep_->getDrawingMode());

	if (rep_->getColorProcessor() != 0)
	{
		custom_color_ = rep_->getColorProcessor()->getDefaultColor();
		setColor(custom_color_label, custom_color_);
	}

	transparency_slider->setValue(rep_->getTransparency());

	coloring_updates_enabled->setChecked(rep_->coloringUpdateEnabled());
	model_updates_enabled->setChecked(rep_->modelUpdateEnabled());

	getAdvancedModelOptions_();
	getAdvancedColoringOptions_();
	checkDrawingPrecision_();
}

void DisplayProperties::selectModel(int index)
{
	if (!model_information_->modelMuteableByDisplayProperties((ModelType)index))
	{
		throw(Exception::InvalidOption(__FILE__, __LINE__, index));
	}

	// enable usage from python
	if (index != model_type_combobox->currentIndex())
	{
		model_type_combobox->setCurrentIndex(index);
		return;
	}

	checkDrawingPrecision_();
}

void DisplayProperties::selectMode(int index)
{
	if (index > VIEW::DRAWING_MODE_TOON)
	{
		BALLVIEW_DEBUG
		return;
	}

	// enable usage from python
	if (index != mode_combobox->currentIndex())
	{
		mode_combobox->setCurrentIndex(index);
		return;
	}
}

void DisplayProperties::selectColoringMethod(int index)
{
	if (!model_information_->coloringMuteableByDisplayProperties((ColoringMethod) index))
	{
		BALLVIEW_DEBUG
		return;
	}

	// enable usage from python
	if (index != coloring_method_combobox->currentIndex())
	{
		coloring_method_combobox->setCurrentIndex(index);
		return;
	}
}

void DisplayProperties::onNotify(Message *message)
	throw()
{
#ifdef BALL_VIEW_DEBUG
	Log.error() << "DisplayProperties::onNotify " << message << std::endl;
#endif
	// new molecule => build graphical representation and notify scene
	if (RTTI::isKindOf<CompositeMessage>(*message))
	{
		CompositeMessage *composite_message = RTTI::castTo<CompositeMessage>(*message);
		if (!create_representations_for_new_molecules_ ||
				composite_message->getType() != CompositeMessage::NEW_MOLECULE)
		{
			return;
		}

		// generate graphical representation
		createRepresentationMode();
		List<Composite*> clist;
		clist.push_back(composite_message->getComposite());
		createRepresentation(clist);
		return;
	}

	if (RTTI::isKindOf<ShowDisplayPropertiesMessage>(*message))
	{
		checkMenu(*getMainControl());
		show();
		return;
	}

	if (RTTI::isKindOf<RepresentationMessage>(*message))
	{
		switch (((RepresentationMessage*) message)->getType())
		{
			case RepresentationMessage::SELECTED:
			{
				Representation* rep = ((RepresentationMessage*) message)->getRepresentation();
				if (rep == 0) 
				{	
					createRepresentationMode();
					return;
				}
				if (rep->getModelType() >= MODEL_UNKNOWN) return;
				modifyRepresentationMode(rep);
				return;
			}
			case RepresentationMessage::REMOVE:
			{
				createRepresentationMode();
				return;
			}

			case RepresentationMessage::STARTED_UPDATE:
			case RepresentationMessage::FINISHED_UPDATE:
			case RepresentationMessage::UPDATE:
			{
				checkMenu(*getMainControl());
				return;
			}

			default:
				return;
		}
		return;
	}

	if (RTTI::isKindOf<ControlSelectionMessage>(*message))
	{
		// disable apply button if selection is empty
		createRepresentationMode();
		return;
	}

	if (RTTI::isKindOf<FinishedSimulationMessage>(*message))
	{
		checkMenu(*getMainControl());
		return;
	}

	if (RTTI::isKindOf<CreateRepresentationMessage>(*message))
	{
		CreateRepresentationMessage* crm = (CreateRepresentationMessage*) message;
		if (crm->getComposites().size() == 0) return;
		model_type_combobox->setCurrentIndex(crm->getModelType());
		coloring_method_combobox->setCurrentIndex(crm->getColoringMethod());
		createRepresentationMode();
		createRepresentation(crm->getComposites());
	}
}

void DisplayProperties::apply()
{
	// no molecular or representation selection present 
	if (getMainControl()->getMolecularControlSelection().size() == 0 && rep_ == 0)
	{
		return;
	}

	setStatusbarText("building model...");

	if (changed_selection_color_)
	{
		BALL_SELECTED_COLOR_CHANGE_TIME = PreciseTime::now();
		getMainControl()->getRepresentationManager().rebuildAllRepresentations();
	}

	create_button->setEnabled(false);
	modify_button->setEnabled(false);

	if (rep_ == 0)
	{
		createRepresentation(getMainControl()->getMolecularControlSelection());
	}
	else
	{
		applyTo_(rep_);
	}

	modify_button->setEnabled(!getMainControl()->isBusy());
}


void DisplayProperties::editColor()
{
	custom_color_.set(chooseColor(custom_color_label));
}

void DisplayProperties::editSelectionColor()
{
	BALL_SELECTED_COLOR.set(chooseColor(selection_color_label));
	changed_selection_color_ = true;
}

// ------------------------------------------------------------------------
// Model Processor methods
// ------------------------------------------------------------------------
void DisplayProperties::applyModelSettings_(Representation& rep)
{
	if (model_settings_ == 0) return;

	ModelType current_type = (ModelType) model_type_combobox->currentIndex();
	if (rep.getModelProcessor() == 0 ||
			rep.getModelType() != current_type ||
			!rep.modelUpdateEnabled())
	{
		rep.setModelProcessor(model_settings_->createModelProcessor(current_type));
		rep.setModelType((ModelType)model_type_combobox->currentIndex());
	}

	rep.setDrawingMode((DrawingMode)mode_combobox->currentIndex());

	if (custom_precision_button->isChecked())
	{
		rep.setSurfaceDrawingPrecision(((float)precision_slider->value()) / 10.0);
	}
	else
	{
		rep.setDrawingPrecision((DrawingPrecision) precision_combobox->currentIndex());

		if (model_information_->isSurfaceModel(current_type))
		{
			rep.setSurfaceDrawingPrecision(SurfaceDrawingPrecisions[precision_combobox->currentIndex()]);
		}
	}

	model_settings_->applySettingsTo(*rep.getModelProcessor());
}


void DisplayProperties::applyColoringSettings_(Representation& rep)
{
	if (coloring_settings_ == 0) return;

	ColoringMethod current_coloring = (ColoringMethod) coloring_method_combobox->currentIndex();

 	if (rep.getColorProcessor() == 0 ||
 			rep.getColoringMethod() != current_coloring ||
 			!rep.coloringUpdateEnabled())
	{
		if (coloring_settings_ != 0)
		{
			rep.setColorProcessor(coloring_settings_->createColorProcessor(current_coloring));
		}
		else
		{
			rep.setColorProcessor(new ColorProcessor());
		}
		rep.setColoringMethod(current_coloring);
	}

	Size transparency = transparency_slider->value();
	custom_color_ = getColor(custom_color_label);
	custom_color_.setAlpha(255 - transparency);
	rep.setTransparency(transparency);

	ColorProcessor* cp = rep.getColorProcessor();
	coloring_settings_->applySettingsTo(*cp);
	float min_spacing = 2.;
	if (rep.getModelType() == MODEL_SE_SURFACE)
	{
		min_spacing = 2.62;
	}
	else if (rep.getModelType() == MODEL_SA_SURFACE)
	{
		min_spacing = 2.62 + ((AddSurfaceModel*)rep.getModelProcessor())->getProbeRadius();
	}
	cp->setMinGridSpacing(min_spacing);
	cp->setDefaultColor(custom_color_);
}


Representation* DisplayProperties::createRepresentation(const List<Composite*>& composites, bool hidden)
{
	if (composites.size() == 0) return 0;

	// workaround for MSVC: crashed otherwise
	Composite* first_composite = *composites.begin();
	if (first_composite == 0) return 0 ;

	// create a new Representation
	rep_ = new Representation();
	applyColoringSettings_(*rep_);
	applyModelSettings_(*rep_);

	// stupid, but must be: create a List with const Composites!
	List<const Composite*> temp_composites;
	List<Composite*>::ConstIterator it = composites.begin();
	for (; it != composites.end(); it++)
	{
		temp_composites.push_back(*it);
	}
	rep_->setComposites(temp_composites);
	if (hidden) rep_->setHidden(true);

	// this is not straight forward, but we have to prevent a second rendering run in the Scene...
	// the insertion into the RepresentationManager is needed to allow the Representation::update
	getMainControl()->getRepresentationManager().insert(*rep_, false);
		
	Representation* repx = rep_;
	// now we can add the Representation to the GeometricControl
	notify_(new RepresentationMessage(*rep_, RepresentationMessage::ADD_TO_GEOMETRIC_CONTROL));

	rep_ = repx;

	// no refocus, if a this is not the only Representation
	if ((getMainControl()->getRepresentationManager().getRepresentations().size() < 2))
	{
		notify_(new CompositeMessage(*first_composite, CompositeMessage::CENTER_CAMERA));
	}

	if (hidden) return rep_;

	rep_->update(true);
	
	return rep_;
}


void DisplayProperties::applyTo_(Representation* rep)
{
	rep_ = rep;
	ModelType mt = (ModelType) model_type_combobox->currentIndex();

	bool 	rebuild_representation = rep_->getModelType() != mt  ||
																 advanced_options_modified_  ||
																 changed_selection_color_;

	if (model_information_->isSurfaceModel(mt))
	{
		// workaround, didnt work right otherwise: (just let it this way)
		rebuild_representation |= 
			(float)rep_->getSurfaceDrawingPrecision() != (float)precision_slider->value() / 10.0;
	}
	else
	{
		DrawingPrecision dp = (DrawingPrecision)precision_combobox->currentIndex();
		rebuild_representation |= (rep_->getDrawingPrecision() != dp);
	}

	if (coloring_updates_enabled->isChecked())
	{
		applyColoringSettings_(*rep_);
	}
	
	if (rebuild_representation && model_updates_enabled->isChecked())
	{
		applyModelSettings_(*rep_);
		advanced_options_modified_ = false;
	}
	else
	{
		Size transparency = transparency_slider->value();

		if (!coloring_updates_enabled->isChecked() &&
		    rep_->getTransparency() != transparency)
		{
			GeometricObjectList::iterator it = rep_->getGeometricObjects().begin();
			for (; it != rep_->getGeometricObjects().end(); it++)
			{
				Mesh* mesh = dynamic_cast<Mesh*> (*it);
				if (mesh != 0)
				{
					mesh->setAlphas(255 - transparency);
				}
				else
				{
					(**it).getColor().setAlpha(255 - transparency);
				}
			}
		}
	}

	rep_->enableModelUpdate(model_updates_enabled->isChecked());
	rep_->enableColoringUpdate(coloring_updates_enabled->isChecked());
	
	if (coloring_updates_enabled->isChecked())
	{
		applyColoringSettings_(*rep_);
	}

	if (model_updates_enabled->isChecked())
	{
		applyModelSettings_(*rep_);
	}
	rep_->update(rebuild_representation);

	changed_selection_color_ = false;
}


void DisplayProperties::transparencySliderChanged()
{
	String text = String((int)((float)transparency_slider->value()/2.55));
	text += String("%");
	transparency_label->setText(text.c_str());
}

void DisplayProperties::precisionSliderChanged()
{
	String text = String(((float)precision_slider->value()) / 10.0).trimRight("0");
	if (text.right(1) == ".") text = text + "0";

	custom_precision_label->setText(text.c_str());
	custom_precision_button->setChecked(true);
}

void DisplayProperties::coloringOptionsPressed()
{
	if (preferences_ == 0) return;

	preferences_->showEntry(coloring_settings_->getEntryFor((ColoringMethod) coloring_method_combobox->currentIndex()));
	preferences_->show();
}

void DisplayProperties::modelOptionsPressed()
{
	if (preferences_ == 0) return;

	preferences_->showEntry(model_settings_->getEntryFor((ModelType) model_type_combobox->currentIndex()));
	preferences_->show();
}

void DisplayProperties::precisionBoxChanged(int index)
{
	presets_precision_button->setChecked(true);

	if (index < DRAWING_PRECISION_LOW ||
			index > DRAWING_PRECISION_ULTRA)
	{
		throw(Exception::InvalidOption(__FILE__, __LINE__, index));
	}

	String label = createFloatString(SurfaceDrawingPrecisions[index], 1);
	presets_precision_label->setText(label.c_str());
}

void DisplayProperties::checkDrawingPrecision_()
	throw()
{
	ModelType mt = (ModelType)model_type_combobox->currentIndex();

	if (!model_information_->modelMuteableByDisplayProperties(mt) ||
	    !model_updates_enabled->isChecked())
	{
		return;
	}

	bool is_surface = model_information_->isSurfaceModel(mt);
	custom_precision_button->setEnabled(is_surface);
	precision_slider->setEnabled(is_surface);

	if (!is_surface)
	{
		presets_precision_button->setChecked(true);
	}

	if (rep_ == 0) return;
		
	if (is_surface &&
			rep_->getSurfaceDrawingPrecision() != -1)
	{
		custom_precision_button->setChecked(true);
		setSurfaceDrawingPrecision(rep_->getSurfaceDrawingPrecision());
		return;
	}
	
	if (rep_->getDrawingPrecision() != DRAWING_PRECISION_INVALID)
	{
		setDrawingPrecision(rep_->getDrawingPrecision());
	}
}

void DisplayProperties::getAdvancedColoringOptions_()
	throw()
{
	if (rep_ == 0 ||
			rep_->getColorProcessor() == 0 ||
			coloring_method_combobox->currentIndex() == COLORING_CUSTOM) 
	{
		return;
	}

	coloring_settings_->getSettings(*rep_->getColorProcessor());
}

void DisplayProperties::getAdvancedModelOptions_()
	throw()
{
	if (rep_ == 0 ||
			rep_->getModelProcessor() == 0) 
	{
		return;
	}
	
	model_settings_->getSettings(*rep_->getModelProcessor());
}
			
void DisplayProperties::applyPreferences()
	throw()
{
	if (rep_ == 0) 
	{
		return;
	}

	advanced_options_modified_ = true;

	const QWidget* current_page = getMainControl()->getPreferences()->currentEntry();

	if (current_page == model_settings_ ||
			current_page == coloring_settings_) 
	{
		apply();
		advanced_options_modified_ = false;
	}
}

bool DisplayProperties::getSettingsFromString(const String& data)
	throw()
{
	vector<String> fields;
	if (data.split(fields) < 6 ||
			!model_information_->modelMuteableByDisplayProperties((ModelType)fields[0].toUnsignedInt())) 
	{
		return false;
	}
	
	try
	{
		selectModel(fields[0].toUnsignedInt());
		selectMode(fields[1].toUnsignedInt());
		setDrawingPrecision(fields[2].toUnsignedInt());
		setSurfaceDrawingPrecision(fields[3].toFloat());
		selectColoringMethod(fields[4].toUnsignedInt());
		setTransparency(fields[5].toInt());
		return true;
	}
	catch(...)
	{
	}

	return false;
}

void DisplayProperties::createRepresentation(String data_string, const vector<const Composite*>& new_systems)
{
	try
	{
		vector<String> string_vector;

		// Representation0=1;3 2 2 6.500000 0 0 [2]|Color|H
		// 								 ^ 																	System Number
		// 								         ^            							Model Settings
		// 								         							 ^            Composites numbers
		// 								         							     ^        Custom Color
		// 								         							     			^   Hidden Flag

		// split off information of system number
		data_string.split(string_vector, ";");
		if (string_vector.size() < 2)
		{
			BALLVIEW_DEBUG
			return;
		}

		Position system_pos = string_vector[0].toUnsignedInt();

		// split off between representation settings and composite numbers
		data_string = string_vector[1];
		vector<String> string_vector2;
		data_string.split(string_vector2, "[]");
		if (string_vector2.size() < 3) 
		{
			BALLVIEW_DEBUG
			return;
		}

		bool hidden = (string_vector2.size() >= 3 && string_vector2[2].has('H'));

		data_string = string_vector2[0];

		// Composites id's per number
		data_string = string_vector2[1];
		data_string.split(string_vector2, ",");
		HashSet<Position> hash_set;
		for (Position p = 0; p < string_vector2.size(); p++)
		{
			hash_set.insert(string_vector2[p].toUnsignedInt());
		}

		if (hash_set.size() == 0)
		{
			BALLVIEW_DEBUG;
			return;
		}

		if (system_pos >= new_systems.size())
		{
			Log.error() << "Error while reading project file, invalid structure for Representation! Aborting..." << std::endl;
			return;
		}

		// custom color
		data_string = string_vector[1];
		if (data_string.has('|'))
		{
			data_string.split(string_vector2, "|");
			ColorRGBA color;
			color = string_vector2[1];
			setCustomColor(color);
		}

		Composite* composite = (Composite*) new_systems[system_pos];

		Position current = 0;
		List<Composite*> c_list;
		Composite::CompositeIterator ccit = composite->beginComposite();
		for (; +ccit; ++ccit)
		{
			if (hash_set.has(current)) c_list.push_back(&*ccit);
			current++;
		}

		if (!getSettingsFromString(data_string))
		{
			BALLVIEW_DEBUG;
			Log.error() << "data_string " << std::endl;
			return;
		}

		createRepresentation(c_list, hidden);
	}
	catch(...)
	{
	}
}


void DisplayProperties::setSurfaceDrawingPrecision(float value)
{
	if (value < 0.1) return;
	precision_slider->setValue((int)(value * 10.0));
	
	bool is_s = model_information_->isSurfaceModel((ModelType)model_type_combobox->currentIndex());
	custom_precision_button->setChecked(is_s);
	presets_precision_button->setChecked(!is_s);
}
		
void DisplayProperties::setDrawingPrecision(int value)
{
	precision_combobox->setCurrentIndex(value);
	precisionBoxChanged(value);
	presets_precision_button->setChecked(true);
}

void DisplayProperties::setTransparency(int value)
{
	transparency_slider->setValue(value);
}

void DisplayProperties::setCustomColor(const ColorRGBA& color)
{
	custom_color_ = color;
	setColor(custom_color_label, custom_color_);
}
	
void DisplayProperties::coloringUpdatesChanged()
{
	bool enabled = coloring_updates_enabled->isChecked();

	coloring_method_combobox->setEnabled(enabled);
	custom_button->setEnabled(enabled);
	coloring_options->setEnabled(enabled);
}

void DisplayProperties::modelUpdatesChanged()
{
	bool enabled = model_updates_enabled->isChecked();

	model_type_combobox->setEnabled(enabled);
	model_options->setEnabled(enabled);
	resolution_group->setEnabled(enabled);
}

void DisplayProperties::setModelSettingsDialog(ModelSettingsDialog* dialog)
{
	if (model_settings_ != 0) delete model_settings_;

	model_settings_ = dialog;
}

void DisplayProperties::setColoringSettingsDialog(ColoringSettingsDialog* dialog)
{
	if (coloring_settings_ != 0) delete coloring_settings_;

	coloring_settings_ = dialog;
}
	
} } // namespaces
