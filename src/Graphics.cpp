// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

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
#include "ObjectLoader.h"
#include "Render.h"
#include "Sky.h"
#include "System.h"
#include "Terrain.h"
#include "WorldEntity.h"

namespace Sear {

Graphics::Graphics(System * system) :
  _system(system),
  _renderer(NULL),
  _character(NULL),
  _camera(NULL),
  _terrain(NULL),
  _sky(NULL),
  _num_frames(0),
  _frame_time(0)
{}

Graphics::~Graphics() {}

void Graphics::init() {
  readConfig();
  Log::writeLog("Initialising Terrain", Log::LOG_DEFAULT);
  _renderer = new GL(_system, this);
  _renderer->init();
  _terrain = new ROAM(_system, _renderer);
  if (!_terrain->init()) {
    Log::writeLog("Error initialising Terrain. Suggest Restart!", Log::LOG_ERROR);
  }
  _sky = new SkyBox(_system, _renderer);
  if (!_sky->init()) {
    Log::writeLog("Render: Error - Could not initialise Sky Box", Log::LOG_ERROR);
  }
  _camera = new Camera();
  _camera->init();
  _camera->registerCommands(_system->getConsole());

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
}

void Graphics::drawScene(const std::string& command, bool select_mode, float time_elapsed) {
  if (!_renderer) throw Exception("No Render object to render with!");
  if (select_mode) _renderer->resetSelection();
  if (_camera) _camera->updateCameraPos(time_elapsed);

  _renderer->beginFrame();
  
  Eris::World *world = Eris::World::Instance();
  if (_system->checkState(SYS_IN_WORLD) && world) {
    if (!_character) _character = _system->getCharacter();
    WorldEntity *focus = (WorldEntity *)world->getFocusedEntity(); //Get the player character entity
    if (focus) {
      float x = 0.0f, y = 0.0f, z = 0.0f; // Initial camera position
      std::string id = focus->getID();
      orient = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f); // Initial Camera rotation
      orient /= WFMath::Quaternion(WFMath::Vector<3>(0.0f, 0.0f, 1.0f), WFMath::Pi / 2.0f); // Rotate by 90 degrees as WF 0 degrees is East
      WFMath::Point<3> pos = focus->getAbsPos();
      x = -pos.x();
      y = -pos.y();
      z = -pos.z();
      
      // Apply camera rotations
      orient /= WFMath::Quaternion(WFMath::Vector<3>(0.0f, 1.0f, 0.0f), _camera->getElevation());
      orient /= WFMath::Quaternion(WFMath::Vector<3>(0.0f, 0.0f, 1.0f), _camera->getRotation());
      
      if (_character) orient /= WFMath::Quaternion(WFMath::Vector<3>(0.0f, 0.0f, 1.0f),  _character->getAngle());

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
      
      _renderer->translateObject(x, y, z - 2.0f); //Translate to accumulated position - Also adjust so origin is nearer head level
    
      _renderer->applyCharacterLighting(-x, -y, -z + 2.0f);

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
      _render_queue = std::map<std::string, Queue>();
      buildQueues(root, 0, select_mode);
      _renderer->drawQueue(_render_queue, select_mode, time_elapsed);
      if (!select_mode) _renderer->drawMessageQueue(_render_queue);
    }
  } else {
    _renderer->drawSplashScreen();
  }
 
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

void Graphics::buildQueues(WorldEntity *we, int depth, bool select_mode) {
  _model_handler = _system->getModelHandler();	
  if (depth == 0 || we->isVisible()) {
    if (we->getType() != NULL) {
      ObjectProperties *op = we->getObjectProperties();
      if (!op) {
        _model_handler->getModel(we); // Allocates a model and an ObjectProperties
        op = we->getObjectProperties();
      }
      if (!op) return; // Why is this required? getModel should guarantee that op is valid, or it would send an error msg
      if (op->draw_self && select_mode && Frustum::sphereInFrustum(frustum, we, _terrain)) _render_queue[op->select_state].push_back(we);
      if (op->draw_self && !select_mode && Frustum::sphereInFrustum(frustum, we, _terrain)) _render_queue[op->state].push_back(we);
      if (op->draw_members) {
        for (unsigned int i = 0; i < we->getNumMembers(); i++) {
          buildQueues((WorldEntity*)we->getMember(i), depth + 1, select_mode);
        }
      }
    }
  }
}

void Graphics::readConfig() {
  varconf::Variable temp;
  varconf::Config *general = _system->getGeneral();
  Log::writeLog("Loading Renderer Config", Log::LOG_DEFAULT);
  if (!general) {
    Log::writeLog("Graphics: Error - General config object does not exist!", Log::LOG_ERROR);
    return;
  }

  // Setup frame rate detail boundaries
  temp = general->getItem("graphics", KEY_lower_frame_rate_bound);
  _lower_frame_rate_bound = (!temp.is_double()) ? (DEFAULT_lower_frame_rate_bound) : ((double)(temp));
  temp = general->getItem("graphics", KEY_upper_frame_rate_bound);
  _upper_frame_rate_bound = (!temp.is_double()) ? (DEFAULT_upper_frame_rate_bound) : ((double)(temp));
  
}  

void Graphics::writeConfig() {
  varconf::Config *general = _system->getGeneral();
  if (!general) {
    Log::writeLog("Graphics: Error - General config object does not exist!", Log::LOG_ERROR);
    return;
  }
  
  // Save frame rate detail boundaries
  general->setItem("graphics", KEY_lower_frame_rate_bound, _lower_frame_rate_bound);
  general->setItem("graphics", KEY_upper_frame_rate_bound, _upper_frame_rate_bound);
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
  ModelHandler *model_handler = _system->getModelHandler();	
  if (frame_rate < _lower_frame_rate_bound) {
    if (model_handler) model_handler->lowerDetail();
    if (_terrain) _terrain->lowerDetail();
  } else if (frame_rate > _upper_frame_rate_bound) {
    if (model_handler) model_handler->raiseDetail();
    if (_terrain) _terrain->raiseDetail();
  }
}

} /* namespace Sear */
