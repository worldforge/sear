// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Graphics.cpp,v 1.17 2002-10-20 13:22:26 simon Exp $

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

namespace Sear {

Graphics::Graphics(System * system) :
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
render_mode = SPLASH;

}

Graphics::~Graphics() {
  if (_initialised) shutdown();
}

void Graphics::init() {
  if (_initialised) shutdown();
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
  _initialised = false;
}

void Graphics::drawScene(const std::string& command, bool select_mode, float time_elapsed) {
  if (!_renderer) throw Exception("No Render object to render with!");
  if (select_mode) _renderer->resetSelection();
  if (_camera) _camera->updateCameraPos(time_elapsed);

  _renderer->beginFrame();
  switch(render_mode) {
    case (SPLASH): drawSplash(command, select_mode, time_elapsed); break;
    case (SERVER): drawServer(command, select_mode, time_elapsed); break;
    case (ACCOUNT): drawAccount(command, select_mode, time_elapsed); break;
    case (LOBBY): drawLobby(command, select_mode, time_elapsed); break;
    case (CHARACTER): drawCharacter(command, select_mode, time_elapsed); break;
    case (INVENTORY): drawInventory(command, select_mode, time_elapsed); break;
    case (WORLD): drawWorld(command, select_mode, time_elapsed); break;
    default: drawSplash(command, select_mode, time_elapsed); break;
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

void Graphics::drawSplash(const std::string& command, bool select_mode, float time_elapsed){
  _renderer->drawSplashScreen();
}

void Graphics::drawServer(const std::string &command ,bool, float) {
  _renderer->drawSplashScreen();
//  _renderer->stateChange("font");
  //
//  _renderer->setColour(1.0f, 0.0f, 0.0f, 1.0f);
//  _renderer->print(20, 50, "Server Screen", 0);
#ifdef HAVE_GLGOOEY
  Gooey::WindowManager::instance().applicationResized(640, 480);
  Gooey::WindowManager::instance().update();
#endif
#if(0)
  render background
  render connect button
  render disconnect button
  render reconnect button
  render scroller table of servers
  render current server details

#endif
}

void Graphics::drawAccount(const std::string &command ,bool, float) {
  _renderer->drawSplashScreen();
  _renderer->stateChange("font");
  _renderer->setColour(1.0f, 0.0f, 0.0f, 1.0f);
  _renderer->print(20, 50, "Account Screen", 0);
  _renderer->print(20, 400, "Username: ", 0);
  _renderer->print(20, 380, "Password: ", 0);
  _renderer->print(20, 360, "Fullname: ", 0);

}

void Graphics::drawLobby(const std::string &command ,bool, float) {
  _renderer->drawSplashScreen();
  _renderer->stateChange("font");
  _renderer->setColour(1.0f, 0.0f, 0.0f, 1.0f);
  _renderer->print(20, 50, "Lobby Screen", 0);
}

void Graphics::drawCharacter(const std::string &command ,bool, float) {
  _renderer->drawSplashScreen();
  _renderer->stateChange("font");
  _renderer->setColour(1.0f, 0.0f, 0.0f, 1.0f);
  _renderer->print(20, 50, "Character Screen", 0);
}

void Graphics::drawInventory(const std::string &command ,bool, float) {
  _renderer->drawSplashScreen();
  _renderer->stateChange("font");
  _renderer->setColour(1.0f, 0.0f, 0.0f, 1.0f);
  _renderer->print(20, 50, "Inventory Screen", 0);
}

void Graphics::drawWorld(const std::string& command, bool select_mode, float time_elapsed) {
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
    _renderer->stateChange("font");
    _renderer->setColour(1.0f, 0.0f, 0.0f, 1.0f);
    _renderer->print(20, 50, "World Screen", 0);
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
        object_record = object_handler->getObjectRecord("default");
	object_handler->copyObjectRecord(we->getID(), object_record);
        object_record = object_handler->getObjectRecord(we->getID());
        if (object_record) object_record->type = "default";
      }
      if (!object_record) {
        std::cout << "No Record found" << std::endl;	      
        return;   
      }
      object_record->name = we->getName();
      object_record->id = we->getID();

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
	    render_queue[_system->getModelRecords()->getItem(*I, "state")].push_back(Render::QueueItem(object_record, *I));
	    if (we->hasMessages()) message_list.push_back(we);
	  }
          else render_queue[_system->getModelRecords()->getItem(*I, "select_state")].push_back(Render::QueueItem(object_record, *I));
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
  console->registerCommand(SWITCH_MODE, this);
}

void Graphics::runCommand(const std::string &command, const std::string &args) {
  if (command == SWITCH_MODE) {
    if (args == "splash") render_mode = SPLASH;
    else if (args == "server") render_mode = SERVER;
    else if (args == "account") render_mode = ACCOUNT;
    else if (args == "lobby") render_mode = LOBBY;
    else if (args == "character") render_mode = CHARACTER;
    else if (args == "inventory") render_mode = INVENTORY;
    else if (args == "world") render_mode = WORLD;
  }
}

} /* namespace Sear */
