// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: Graphics.cpp,v 1.54 2004-06-24 15:20:13 simon Exp $

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <sage/sage.h>

#include <varconf/Config.h>
#include <Eris/Entity.h>
#include <Eris/World.h>
#include <wfmath/quaternion.h>
#include <wfmath/vector.h>

#include "common/Log.h"
#include "common/Utility.h"
#include "environment/Environment.h"
#include "renderers/GL.h"
#include "renderers/Sprite.h"
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
#include "System.h"
#include "WorldEntity.h"

#include "renderers/RenderSystem.h"
#include "gui/Compass.h"
#include "gui/Workspace.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

 extern void renderDome(float, int, int);
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
  _num_frames(0),
  _frame_time(0),
  _initialised(false),
  m_compass(NULL),
  tr(NULL)
{
}

Graphics::~Graphics() {
  if (_initialised) shutdown();
}

void Graphics::init() {
  if (_initialised) shutdown();
  readConfig();
  _system->getGeneral().sigsv.connect(SigC::slot(*this, &Graphics::varconf_callback));
  
  RenderSystem::getInstance().registerCommands(_system->getConsole());

//  ((GL*)_renderer)->getTextureManager()->registerCommands(_system->getConsole());
//  ((GL*)_renderer)->getStateManager()->registerCommands(_system->getConsole());
  _camera = new Camera();
  _camera->init();
  _camera->registerCommands(_system->getConsole());
  
    m_compass = new Compass(580.f, 50.f);
    m_compass->setup();
    
  _initialised = true;
}

void Graphics::shutdown() {
  writeConfig();

  if (_camera) {
    _camera->shutdown();
    delete _camera;
    _camera = NULL;
  }
  
  delete m_compass;
  _initialised = false;
}

void Graphics::drawScene(const std::string& command, bool select_mode, float time_elapsed) {
  if (!_renderer) {
    std::cerr << "No Render object to render with!" << std::endl;
    return;
  }
  if (select_mode) _renderer->resetSelection();
  if (_camera) _camera->updateCameraPos(time_elapsed);

  _renderer->beginFrame();
  drawWorld(command, select_mode, time_elapsed);

  if (!select_mode) {
    Workspace *ws = _system->getWorkspace();
    if (ws) ws->draw();
    else throw Exception("Error no Workspace object");

    Console *con = _system->getConsole();
    if (con) con->draw(command);
    else throw Exception("Error no Console object");
  }

  if (!select_mode) {
    // Only update on a viewable frame
    _frame_time += time_elapsed;
    _frame_rate = (float)_num_frames++ /_frame_time;
    if (_frame_time > 1.0f) {
      std::string fr = string_fmt(_frame_rate);
      SDL_WM_SetCaption(fr.c_str(), fr.c_str());
      _num_frames = 0;
      _frame_time = 0.0f;
    }
  }
//  updateDetailLevels(_frame_rate);
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

	
  WFMath::Point<3> pos(0,0,0); // Initial camera position
#warning FIXME Should not be treating World as a singleton
  Eris::World *world = Eris::World::getPrimary();
  if (_system->checkState(SYS_IN_WORLD) && world) {
    if (!_character) _character = _system->getCharacter();
    WorldEntity *focus = (WorldEntity *)world->getFocusedEntity(); //Get the player character entity
    if (focus) {
      std::string id = focus->getID();
      static WFMath::Quaternion quaternion_by_90 = WFMath::Quaternion(z_vector, WFMath::Pi / 2.0f);
      orient = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f); // Initial Camera rotation
      orient /= quaternion_by_90; // Rotate by 90 degrees as WF 0 degrees is East
      pos = focus->getAbsPos();
      float x = pos.x(),
            y = pos.y(),
            z = pos.z();
      
      // Apply camera rotations
      orient /= WFMath::Quaternion(y_vector, _camera->getElevation());
      orient /= WFMath::Quaternion(z_vector, _camera->getRotation());
      
      if (_character) orient /= WFMath::Quaternion(z_vector,  _character->getAngle());

      // Draw Sky box, requires the rotation to be done before any translation to keep the camera centered
      if (!select_mode) {
	_renderer->store();
        _renderer->applyQuaternion(orient);
        Environment::getInstance().renderSky();
	_renderer->restore();
      }
      // Translate camera getDist() units away from the character. Allows closups or large views
      _renderer->translateObject(0.0f, _camera->getDistance(), -1.0f);
      _renderer->applyCharacterLighting(0.5, 0, 0.5);
      _renderer->applyQuaternion(orient);
      
      float height = (focus->hasBBox()) ? (focus->getBBox().highCorner().z() - focus->getBBox().lowCorner().z()) : (1.0f);
      _renderer->translateObject(-x, -y, -z - height); //Translate to accumulated position - Also adjust so origin is nearer head level
    
      // _renderer->applyCharacterLighting(x, y, z + height);

      _renderer->getFrustum(frustum);
    }

    _renderer->applyLighting();

    if (!select_mode ) {
      _renderer->store();
  //    _terrain->draw();

  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState("terrain"));
  glEnableClientState(GL_VERTEX_ARRAY);
  Environment::getInstance().renderTerrain(pos);
  glDisableClientState(GL_VERTEX_ARRAY);

      _renderer->restore();
    }
#if(1)
    WorldEntity *root = NULL; 
    if ((root = (WorldEntity *)world->getRootEntity())) {
      if (_character) _character->updateLocals(false);
      _render_queue = Render::QueueMap();
      _message_list = Render::MessageList();
      buildQueues(root, 0, select_mode, _render_queue, _message_list);
      _renderer->drawQueue(_render_queue, select_mode, time_elapsed);
      if (!select_mode) _renderer->drawMessageQueue(_message_list);
    }
#endif
      _renderer->store();
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState("terrain"));
  glEnableClientState(GL_VERTEX_ARRAY);
  Environment::getInstance().renderSea();
  glDisableClientState(GL_VERTEX_ARRAY);
      _renderer->restore();
    
    m_compass->update(_camera->getRotation());
    m_compass->draw(_renderer, select_mode);
  } else {

//    _renderer->applyLighting();
//_renderer->stateChange(_renderer->getStateID("terrain"));
  //    _renderer->translateObject(0.0f, _camera->getDistance(), 0.0f);
//  glEnableClientState(GL_VERTEX_ARRAY);
//  tr->render(WFMath::Point<3>(0,0,0));//x_pos, y_pos, z_pos));
  //glDisableClientState(GL_VERTEX_ARRAY);
    _renderer->drawSplashScreen();
//      _renderer->translateObject(0.0f, _camera->getDistance(), 0.0f);
//    _renderer->applyLighting();
//  glEnableClientState(GL_VERTEX_ARRAY);
//  tr->render(WFMath::Point<3>(0,0,0));//x_pos, y_pos, z_pos));
//  glDisableClientState(GL_VERTEX_ARRAY);
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
        std::string type = we->type();
        varconf::Config::inst()->clean(type);
        object_record = object_handler->getObjectRecord(type);
	object_handler->copyObjectRecord(we->getID(), object_record);
        object_record = object_handler->getObjectRecord(we->getID());
        if (object_record) object_record->type = we->type();
      }
      if (!object_record) {
        std::string parent = we->parent();
        varconf::Config::inst()->clean(parent);
        object_record = object_handler->getObjectRecord(parent);
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
//      if (object_record->low_quality.begin() == object_record->low_quality.end()) std::cout << "Error, no models!" << std::endl;
      // Loop through all models in list
      if (object_record->draw_self) {
        for (ObjectRecord::ModelList::const_iterator I = object_record->low_quality.begin(); I != object_record->low_quality.end(); ++I) {
          // Check if we're visible
          // Change method here
          if (Frustum::sphereInFrustum(frustum, object_record->bbox, object_record->position)) {
          //if (Frustum::orientBBoxInFrustum(frustum, we->getOrientBBox(), object_record->position)) {
            if (!select_mode) {
               // Add to queue by state, then model record
//               assert(_system->getModelRecords().findItem(*I, "state_num"));
               int number = _system->getModelRecords().getItem(*I, "state_num");
//               assert(number > 0);
               render_queue[_system->getModelRecords().getItem(*I, "state_num")].push_back(Render::QueueItem(object_record, *I));
              if (we->hasMessages()) message_list.push_back(we);
            }
            else render_queue[_system->getModelRecords().getItem(*I, "select_state_num")].push_back(Render::QueueItem(object_record, *I));
          }
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
  if (general.getItem("graphics", KEY_lower_frame_rate_bound)) {
    temp = general.getItem("graphics", KEY_lower_frame_rate_bound);
    _lower_frame_rate_bound = (!temp.is_double()) ? (DEFAULT_lower_frame_rate_bound) : ((double)(temp));
  }
  if (general.findItem("graphics", KEY_upper_frame_rate_bound)) {
    temp = general.getItem("graphics", KEY_upper_frame_rate_bound);
    _upper_frame_rate_bound = (!temp.is_double()) ? (DEFAULT_upper_frame_rate_bound) : ((double)(temp));
  }
  
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
}

void Graphics::writeComponentConfig() {
  if (_renderer) _renderer->writeConfig();
//  if (_camera) _camera->writeConfig();
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
  console->registerCommand("invalidate", this);
}

void Graphics::runCommand(const std::string &command, const std::string &args) {
  if (command == "invalidate") {
    RenderSystem::getInstance().invalidate();
    Environment::getInstance().invalidate();
  }
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
