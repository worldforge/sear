// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Graphics.cpp,v 1.26 2003-03-23 19:51:49 simon Exp $

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <varconf/Config.h>
#include <Eris/Entity.h>
#include <Eris/World.h>
#include <wfmath/quaternion.h>
#include <wfmath/vector.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "renderers/GL.h"
#include "sky/SkyBox.h"
#include "terrain/ROAM.h"

#include "Camera.h"
#include "Character.h"
#include "Console.h"
#include "Exception.h"
#include "Frustum.h"
#include "Model.h"
#include "ModelHandler.h"
#include "ModelRecord.h"
#include "ObjectRecord.h"
#include "ObjectHandler.h"
#include "Render.h"
#include "Sky.h"
#include "System.h"
#include "Terrain.h"
#include "WorldEntity.h"

#include "gui/ServerGui.h"

#ifdef HAVE_GLGOOEY

#include "glgooey/WindowManager.h"
#include "glgooey/Rectangle.h"
#include "glgooey/FrameWindow.h"
#include "glgooey/CheckBox.h"
#include "glgooey/CheckBoxGroup.h"
#include "glgooey/Button.h"
#include "glgooey/Panel.h"
#include "glgooey/EditField.h"
#include "glgooey/MultiTextButton.h"
#include "glgooey/Font.h"
#include "glgooey/ScrollBar.h"
#include "glgooey/ListBox.h"
#include "glgooey/ListControl.h"
#include "glgooey/StaticText.h"
#include "glgooey/ProgressBar.h"
#include "glgooey/ComplexGridLayouter.h"
#include "glgooey/StaticBitmap.h"
#include "glgooey/TimeManager.h"
#endif

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

static const std::string GRAPHICS = "graphics";
	
static const std::string DEFAULT = "default";
static const std::string FONT = "font";
static const std::string STATE = "state";
static const std::string SELECT = "select_state";
	  // Consts
  static const int sleep_time = 5000;
  
  // Config key strings
  static const std::string KEY_use_textures = "render_use_textures";
  static const std::string KEY_use_lighting = "render_use_lighting";
  static const std::string KEY_show_fps = "render_show_fps";
  static const std::string KEY_use_stencil = "render_use_stencil";

  static const std::string KEY_lower_frame_rate_bound = "lower_frame_rate_bound";
  static const std::string KEY_upper_frame_rate_bound = "upper_frame_rate_bound";
  
  // Default config values
  static const float DEFAULT_use_textures = true;
  static const float DEFAULT_use_lighting = true;
  static const float DEFAULT_show_fps = true;
  static const float DEFAULT_use_stencil = true;

  static const float DEFAULT_lower_frame_rate_bound = 25.0f;
  static const float DEFAULT_upper_frame_rate_bound = 30.0f;
 
Graphics::Graphics(System *system) :
  _system(system),
  _renderer(NULL),
  _character(NULL),
  _camera(NULL),
  _terrain(NULL),
  _sky(NULL),
  _num_frames(0),
  _frame_time(0),
  _initialised(false)
{
}

Graphics::~Graphics() {
  if (_initialised) shutdown();
}

void Graphics::init() {
  if (_initialised) shutdown();
  readConfig();
  _system->getGeneral().sigsv.connect(SigC::slot(*this, &Graphics::varconf_callback));
  _renderer = new GL(_system, this);
  _renderer->init();
  ((GL*)_renderer)->getTextureManager()->registerCommands(_system->getConsole());
 _camera = new Camera();
  _camera->init();
  _camera->registerCommands(_system->getConsole());
  sg = new ServerGui();
  _initialised = true;
}

void Graphics::shutdown() {
  writeConfig();

  if (_renderer) {
    _renderer->shutdown();
    delete _renderer;
    _renderer = NULL;
  }
    
  if (_terrain) {
    _terrain->shutdown();
    delete _terrain;
    _terrain = NULL;
  }
  if (_sky) {
    _sky->shutdown();
    delete _sky;
    _sky = NULL;
  }
  if (_camera) {
    _camera->shutdown();
    delete _camera;
    _camera = NULL;
  }
  if (sg) {
    delete sg;
    sg = NULL;
  }
  _initialised = false;
}
void Graphics::initST() {
_terrain = new ROAM(_system, _renderer);
  if (!_terrain->init()) {
    Log::writeLog("Error initialising Terrain. Suggest Restart!", Log::LOG_ERROR);
  }
  _sky = new SkyBox(_system, _renderer);
  if (!_sky->init()) {
    Log::writeLog("Render: Error - Could not initialise Sky Box", Log::LOG_ERROR);
  }
}
void Graphics::drawScene(const std::string& command, bool select_mode, float time_elapsed) {
  if (!_renderer) throw Exception("No Render object to render with!");
  if (select_mode) _renderer->resetSelection();
  if (_camera) _camera->updateCameraPos(time_elapsed);

  _renderer->beginFrame();
  drawWorld(command, select_mode, time_elapsed); 

  if (!select_mode) {
    Console *con = _system->getConsole();
    if (con) con->draw(command);
    else throw Exception("Error no Console object");
  }

  if (!select_mode) {
    // Only update on a viewable frame
    _frame_time += time_elapsed;
    _frame_rate = (float)_num_frames++ /_frame_time;
    if (_renderer->checkState(Render::RENDER_FPS))  _renderer->drawFPS(_frame_rate);
    if (_frame_time > 1.0f) {
      _num_frames = 0;
      _frame_time = 0;
    }
  }
  updateDetailLevels(_frame_rate);
  if (!select_mode) _renderer->renderActiveName();
  _renderer->endFrame(select_mode);
}

void Graphics::drawWorld(const std::string& command, bool select_mode, float time_elapsed) {
  static WFMath::Vector<3> y_vector = WFMath::Vector<3>(0.0f, 1.0f, 0.0f);
  static WFMath::Vector<3> z_vector = WFMath::Vector<3>(0.0f, 0.0f, 1.0f);
/*
Camera coords
//Should be stored in camera object an updated as required
x = cos elevation * cos rotation * distance * distance;
y = cos elevation * din rotation * distance * distance;
z = sin elevation * distance;

((CAMERA + CHAR_POS) - ENTITY_POS)^2 = D^2

Compare D^2 to choose what detail level to use

*/

	
  Eris::World *world = Eris::World::Instance();
  if (_system->checkState(SYS_IN_WORLD) && world) {
    if (!_character) _character = _system->getCharacter();
    WorldEntity *focus = (WorldEntity *)world->getFocusedEntity(); //Get the player character entity
    if (focus) {
      float x = 0.0f, y = 0.0f, z = 0.0f; // Initial camera position
      std::string id = focus->getID();
      static WFMath::Quaternion quaternion_by_90 = WFMath::Quaternion(z_vector, WFMath::Pi / 2.0f);
      orient = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f); // Initial Camera rotation
      orient /= quaternion_by_90; // Rotate by 90 degrees as WF 0 degrees is East
      WFMath::Point<3> pos = focus->getAbsPos();
      x = -pos.x();
      y = -pos.y();
      z = -pos.z();
      
      // Apply camera rotations
      orient /= WFMath::Quaternion(y_vector, _camera->getElevation());
      orient /= WFMath::Quaternion(z_vector, _camera->getRotation());
      
      if (_character) orient /= WFMath::Quaternion(z_vector,  _character->getAngle());

      // Draw Sky box, requires the rotation to be done before any translation to keep the camera centered
      if (!select_mode && _sky) {
	_renderer->store();
        _renderer->applyQuaternion(orient);
        _sky->draw(); //Draw the sky box
	_renderer->restore();
      }

      // Translate camera getDist() units away from the character. Allows closups or large views
      _renderer->translateObject(0.0f, _camera->getDistance(), 0.0f);
      _renderer->applyQuaternion(orient);
      
      if (_terrain) z -= _terrain->getHeight(-x, -y);
      float height = (focus->hasBBox()) ? (focus->getBBox().highCorner().z() - focus->getBBox().lowCorner().z()) : (1.0f);
      _renderer->translateObject(x, y, z - height); //Translate to accumulated position - Also adjust so origin is nearer head level
    
      _renderer->applyCharacterLighting(-x, -y, -z + height);

      _renderer->getFrustum(frustum);
    }

    _renderer->applyLighting();

    if (!select_mode && _terrain) {
      _renderer->store();
      _terrain->draw();
      _renderer->restore();
    }
    WorldEntity *root = NULL; 
    if ((root = (WorldEntity *)world->getRootEntity())) {
      if (_character) _character->updateLocals(false);
      _render_queue = Render::QueueMap();
      _message_list = Render::MessageList();
      buildQueues(root, 0, select_mode, _render_queue, _message_list);
      _renderer->drawQueue(_render_queue, select_mode, time_elapsed);
      if (!select_mode) _renderer->drawMessageQueue(_message_list);
    }
  } else {
    _renderer->drawSplashScreen();
//    _renderer->stateChange(FONT);
//    _renderer->setColour(1.0f, 0.0f, 0.0f, 1.0f);
//    _renderer->print(20, 50, "World Screen", 0);
  }
}


void Graphics::buildQueues(WorldEntity *we, int depth, bool select_mode, Render::QueueMap &render_queue, Render::MessageList &message_list) {
  _model_handler = _system->getModelHandler();
  we->checkActions(); // See if model animations need to be updated
  if (depth == 0 || we->isVisible()) {
    if (we->getType() != NULL) {
      // TODO there must be a better way of doing this - after the first time round for each object, it should only require one call
      ObjectHandler *object_handler = _system->getObjectHandler();
      ObjectRecord *object_record = object_handler->getObjectRecord(we->getID());
      if (object_record && object_record->type.empty()) object_record->type = we->getID();
      if (!object_record) {
        object_record = object_handler->getObjectRecord(we->type());
	object_handler->copyObjectRecord(we->getID(), object_record);
        object_record = object_handler->getObjectRecord(we->getID());
        if (object_record) object_record->type = we->type();
      }
      if (!object_record) {
        object_record = object_handler->getObjectRecord(we->parent());
	object_handler->copyObjectRecord(we->getID(), object_record);
        object_record = object_handler->getObjectRecord(we->getID());
        if (object_record) object_record->type = we->parent();
      }
      if (!object_record) {
        object_record = object_handler->getObjectRecord(DEFAULT);
	object_handler->copyObjectRecord(we->getID(), object_record);
        object_record = object_handler->getObjectRecord(we->getID());
        if (object_record) object_record->type = DEFAULT;
      }
      if (!object_record) {
        std::cout << "No Record found" << std::endl;	      
        return;   
      }
      object_record->name = we->getName();
      object_record->id = we->getID();
      object_record->entity = we;

      if (we->hasBBox()) {
        object_record->bbox = we->getBBox();
      } else {
//        WFMath::Point<3> lc = WFMath::Point<3>(0.0f, 0.0f, 0.0f);
//        WFMath::Point<3> hc = WFMath::Point<3>(1.0f, 1.0f, 1.0f);
//        object_record->bbox = WFMath::AxisBox<3>(lc, hc);
      }
      // Hmm, might be better to explicity link to object.
      // calls only required if Pos, or orientation changes - how can we tell?
      object_record->position = we->getAbsPos();
      object_record->orient = we->getAbsOrient();
      // TODO determine what model queue to use.
      // TODO if queue is empty switch to another
      for (ObjectRecord::ModelList::const_iterator I = object_record->low_quality.begin(); I != object_record->low_quality.end(); I++) {
        if (Frustum::sphereInFrustum(frustum, object_record->bbox, object_record->position, _terrain)) {
          if (!select_mode) {
	    render_queue[_system->getModelRecords().getItem(*I, STATE)].push_back(Render::QueueItem(object_record, *I));
	    if (we->hasMessages()) message_list.push_back(we);
	  }
          else render_queue[_system->getModelRecords().getItem(*I, SELECT)].push_back(Render::QueueItem(object_record, *I));
	}
      }
      if (object_record->draw_members) {
        for (unsigned int i = 0; i < we->getNumMembers(); ++i) {
          buildQueues((WorldEntity*)we->getMember(i), depth + 1, select_mode, render_queue, message_list);
        }
      }
    }
  }
}

void Graphics::readConfig() {
  varconf::Variable temp;
  varconf::Config &general = _system->getGeneral();

  // Setup frame rate detail boundaries
  temp = general.getItem("graphics", KEY_lower_frame_rate_bound);
  _lower_frame_rate_bound = (!temp.is_double()) ? (DEFAULT_lower_frame_rate_bound) : ((double)(temp));
  temp = general.getItem("graphics", KEY_upper_frame_rate_bound);
  _upper_frame_rate_bound = (!temp.is_double()) ? (DEFAULT_upper_frame_rate_bound) : ((double)(temp));
  
}  

void Graphics::writeConfig() {
  varconf::Config &general = _system->getGeneral();
  
  // Save frame rate detail boundaries
  general.setItem("graphics", KEY_lower_frame_rate_bound, _lower_frame_rate_bound);
  general.setItem("graphics", KEY_upper_frame_rate_bound, _upper_frame_rate_bound);
}  

void Graphics::readComponentConfig() {
  if (_renderer) _renderer->readConfig();
  if (_camera) _camera->readConfig();
  if (_camera) _camera->writeConfig();
  if (_terrain) _terrain->readConfig();
  if (_sky) _sky->readConfig();
}

void Graphics::writeComponentConfig() {
  if (_renderer) _renderer->writeConfig();
//  if (_camera) _camera->writeConfig();
  if (_terrain) _terrain->writeConfig();
  if (_sky) _sky->writeConfig();
}

void Graphics::updateDetailLevels(float frame_rate) {
// TODO re-implement this
//  ModelHandler *model_handler = _system->getModelHandler();	
//  if (frame_rate < _lower_frame_rate_bound) {
//    if (model_handler) model_handler->lowerDetail();
//    if (_terrain) _terrain->lowerDetail();
//  } else if (frame_rate > _upper_frame_rate_bound) {
//    if (model_handler) model_handler->raiseDetail();
//    if (_terrain) _terrain->raiseDetail();
//  }
}

void Graphics::registerCommands(Console * console) {
}

void Graphics::runCommand(const std::string &command, const std::string &args) {
}

void Graphics::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  varconf::Variable temp;
  if (section == GRAPHICS) {
    if (key == KEY_lower_frame_rate_bound) {
      temp = config.getItem(GRAPHICS, KEY_lower_frame_rate_bound);
      _lower_frame_rate_bound = (!temp.is_double()) ? (DEFAULT_lower_frame_rate_bound) : ((double)(temp));
    }
    else if (key == KEY_upper_frame_rate_bound) {
      temp = config.getItem(GRAPHICS, KEY_upper_frame_rate_bound);
      _upper_frame_rate_bound = (!temp.is_double()) ? (DEFAULT_upper_frame_rate_bound) : ((double)(temp));
    }
  }
}

} /* namespace Sear */
