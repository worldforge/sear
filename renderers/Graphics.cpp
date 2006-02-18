// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: Graphics.cpp,v 1.42 2006-02-18 23:09:12 simon Exp $

#include <sigc++/object_slot.h>

#include <sage/sage.h>

#include <varconf/Config.h>
#include <Eris/Entity.h>
#include <Eris/View.h>
#include <wfmath/quaternion.h>
#include <wfmath/vector.h>

#include "common/Log.h"
#include "common/Utility.h"
#include "environment/Environment.h"
#include "src/Character.h"
#include "src/Console.h"
#include "loaders/ModelSystem.h"
#include "loaders/Model.h"
#include "loaders/ModelRecord.h"
#include "loaders/ObjectRecord.h"
#include "loaders/ObjectHandler.h"
#include "src/System.h"
#include "src/WorldEntity.h"
#include "src/client.h"
#include "gui/Compass.h"
#include "guichan/Workarea.h"

#include "Graphics.h"
#include "Camera.h"
#include "Sprite.h"
#include "Frustum.h"
#include "Light.h"
#include "LightManager.h"
#include "GL.h"
#include "RenderSystem.h"
#include "Render.h"
#include "CameraSystem.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

static bool c_select = false;

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

  static const std::string KEY_fire_ac = "fire_attenuation_constant";
  static const std::string KEY_fire_al = "fire_attenuation_linear";
  static const std::string KEY_fire_aq = "fire_attenuation_quadratic";

  static const std::string KEY_fire_amb_red = "fire_ambient_red";
  static const std::string KEY_fire_amb_green = "fire_ambient_green";
  static const std::string KEY_fire_amb_blue = "fire_ambient_blue";
  static const std::string KEY_fire_amb_alpha = "fire_ambient_alpha";
  static const std::string KEY_fire_diff_red = "fire_diffuse_red";
  static const std::string KEY_fire_diff_green = "fire_diffuse_green";
  static const std::string KEY_fire_diff_blue = "fire_diffuse_blue";
  static const std::string KEY_fire_diff_alpha = "fire_diffuse_alpha";
  static const std::string KEY_fire_spec_red = "fire_specular_red";
  static const std::string KEY_fire_spec_green = "fire_specular_green";
  static const std::string KEY_fire_spec_blue = "fire_specular_blue";
  static const std::string KEY_fire_spec_alpha = "fire_specular_alpha";

//  static const std::string KEY_low_dist = "low_dist";
  static const std::string KEY_medium_dist = "medium_dist";
  static const std::string KEY_high_dist = "high_dist";
 
  // Default config values
  static const float DEFAULT_use_textures = true;
  static const float DEFAULT_use_lighting = true;
  static const float DEFAULT_show_fps = true;
  static const float DEFAULT_use_stencil = true;

  static const float DEFAULT_fire_ac = 0.7f;
  static const float DEFAULT_fire_al = 0.2f;
  static const float DEFAULT_fire_aq = 0.15f;

  static const float DEFAULT_fire_amb_red = 0.0f;
  static const float DEFAULT_fire_amb_green = 0.0f;
  static const float DEFAULT_fire_amb_blue = 0.0f;
  static const float DEFAULT_fire_amb_alpha = 0.0f;
  static const float DEFAULT_fire_diff_red = 1.0f;
  static const float DEFAULT_fire_diff_green = 1.0f;
  static const float DEFAULT_fire_diff_blue = 0.9f;
  static const float DEFAULT_fire_diff_alpha = 0.0f;
  static const float DEFAULT_fire_spec_red = 0.0f;
  static const float DEFAULT_fire_spec_green = 0.0f;
  static const float DEFAULT_fire_spec_blue = 0.0f;
  static const float DEFAULT_fire_spec_alpha = 0.0f;


//  static const float DEFAULT_low_dist    = 1000.0f;
  static const float DEFAULT_medium_dist = 9000.0f;
  static const float DEFAULT_high_dist   = 4500.0f; 

std::string mapAttachSlotToSubmodel(const std::string& s);

Graphics::Graphics(System *system) :
  m_system(system),
  m_renderer(NULL),
  m_num_frames(0),
  m_frame_time(0),
  m_initialised(false),
  m_compass(NULL),
  m_show_names(false),
  m_medium_dist(DEFAULT_medium_dist),
  m_high_dist(DEFAULT_high_dist)
{
}

Graphics::~Graphics() {
  assert (m_initialised == false);
  if (m_initialised) shutdown();
}

void Graphics::init() {
  assert (m_initialised == false);
  if (m_initialised) shutdown();
  // Add callbeck to detect updated options
  m_system->getGeneral().sigsv.connect(SigC::slot(*this, &Graphics::varconf_callback));

  // Create the compass
  m_compass = new Compass(580.f, 50.f);
  m_compass->setup();

  // Create the LightManager    
  m_lm = new LightManager();
  m_lm->init();

  m_state_weather = RenderSystem::getInstance().requestState("weather");
  m_state_terrain = RenderSystem::getInstance().requestState("terrain");
  m_state_select  = RenderSystem::getInstance().requestState("select");
  m_state_cursor  = RenderSystem::getInstance().requestState("cursor");

  m_initialised = true;
}

void Graphics::shutdown() {
  assert(m_initialised == true);
 
  if (m_compass) {
    delete m_compass;
    m_compass = NULL;
  } 

  if (m_lm) {  
    m_lm->shutdown();
    delete m_lm;
    m_lm = NULL;
  }

  m_initialised = false;
}

void Graphics::drawScene(bool select_mode, float time_elapsed) {
  assert(m_renderer != NULL);

  m_renderer->resetSelection();

  // Update camera position
//  if(RenderSystem::getInstance().getCameraSystem()->getCurrentCamera());
  RenderSystem::getInstance().getCameraSystem()->getCurrentCamera()->updateCameraPos(time_elapsed);
  // Do necessary GL initialisation for the frame
  m_renderer->beginFrame();

  // Draw the world!
  drawWorld(select_mode, time_elapsed);

  if (!select_mode) { 
    Workarea * wa = m_system->getWorkarea();
    assert (wa != NULL);
    wa->draw();

    Console *con = m_system->getConsole();
    assert(con);
    con->draw();
  }

  // Update frame rate info
  if (!select_mode) {
    // Only update on a viewable frame
    m_frame_time += time_elapsed;
    m_frame_rate = (float)m_num_frames++ / m_frame_time;
    if (m_frame_time > 1.0f) {
      std::string fr = "Sear: " + string_fmt(m_frame_rate);
      SDL_WM_SetCaption(fr.c_str(), fr.c_str());
      m_num_frames = 0;
      m_frame_time = 0.0f;
    }
  }

  // Render the entity name if available
  if (!select_mode) m_renderer->renderActiveName();

  // Render the mouse cursor
  if (RenderSystem::getInstance().isMouseVisible()) {
    RenderSystem::getInstance().switchState(m_state_cursor);
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    mouse_y = m_renderer->getWindowHeight() - mouse_y - 32;
    glColor3f(1.0f, 1.0f, 1.0f);
    m_renderer->drawTextRect(mouse_x, mouse_y, 32, 32, RenderSystem::getInstance().getMouseCursor());
  }
  // Do any GL bits to finish rendering the frame
  m_renderer->endFrame(select_mode);
}

static WFMath::Vector<3> y_vector = WFMath::Vector<3>(0.0f, 1.0f, 0.0f);
static WFMath::Vector<3> z_vector = WFMath::Vector<3>(0.0f, 0.0f, 1.0f);

void Graphics::setCameraTransform() {
  if (!m_system->checkState(SYS_IN_WORLD)) {
    return;
  }
  Camera *cam = RenderSystem::getInstance().getCameraSystem()->getCurrentCamera();
  assert(cam != NULL);

  // Get the current focus entity
  Eris::Avatar *avatar = m_system->getClient()->getAvatar();
  assert(avatar != NULL);
  WorldEntity *focus = dynamic_cast<WorldEntity *>(avatar->getEntity()); //Get the player character entity
  assert(focus != NULL);

  // Calculate entity height so camera is always at the top
  float height = (focus->hasBBox()) ? (focus->getBBox().highCorner().z() - focus->getBBox().lowCorner().z()) : (1.0f);

  WFMath::Point<3> pos(0,0,0); // Initial camera positiona
  pos = focus->getAbsPos();
  float char_x = pos.x(),
        char_y = pos.y(),
        char_z = pos.z();

  // Adjust height so camera doesn't go underneath the terrain
  // Find terrain height at camera position, and adjust as if camera is at
  // entity feet. (the - height part)
  float terrain_z = Environment::getInstance().getHeight(char_x - cam->getXPos(), char_y + cam->getYPos()) + cam->getZPos() - height;
  // If the calculated terrain height is larger than the current Z for the 
  // camera, adjust to bound
  if (terrain_z > char_z) {
    char_z = terrain_z;
  }

  if (cam->getType() == Camera::CAMERA_CHASE) {
    // Translate camera getDist() units away from the character. Allows closups or large views
    m_renderer->translateObject(0.0f, cam->getDistance(), -1.0f);
  }
  m_renderer->applyCharacterLighting(0.5, 0, 0.5);

  m_renderer->applyQuaternion(m_orient);

  m_renderer->translateObject(-char_x, -char_y, -char_z - height); //Translate to accumulated position - Also adjust so origin is nearer head level
  glGetFloatv(GL_MODELVIEW_MATRIX,&m_modelview_matrix[0][0]);

}

void Graphics::drawWorld(bool select_mode, float time_elapsed) {
  if (c_select) select_mode = true;
  /*
    Camera coords
    //Should be stored in camera object an updated as required
    x = cos elevation * cos rotation * distance * distance;
    y = cos elevation * din rotation * distance * distance;
    z = sin elevation * distance;

    ((CAMERA + CHAR_POS) - ENTITY_POS)^2 = D^2
  
    Compare D^2 to choose what detail level to use
  */

  // Reset enabled light sources
  m_lm->reset();
  // Can we render the world yet?
  if (m_system->checkState(SYS_IN_WORLD)) {
    Camera *cam = RenderSystem::getInstance().getCameraSystem()->getCurrentCamera();
    assert(cam != NULL);
    //if (!m_character) m_character = m_system->getCharacter();
    //WorldEntity *focus = dynamic_cast<WorldEntity *>(view->getTopLevel()); //Get the player character entity

    static WFMath::Quaternion quaternion_by_90 = WFMath::Quaternion(z_vector, WFMath::Pi / 2.0f);
    m_orient = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f); // Initial Camera rotation
    m_orient /= quaternion_by_90; // Rotate by 90 degrees as WF 0 degrees is East

    // Apply camera rotations
    m_orient /= WFMath::Quaternion(y_vector, cam->getElevation());
    m_orient /= WFMath::Quaternion(z_vector, cam->getRotation());
 
    Eris::Avatar *avatar = m_system->getClient()->getAvatar();
    assert(avatar != NULL);
    WorldEntity *focus = dynamic_cast<WorldEntity *>(avatar->getEntity()); //Get the player character entity
    assert(focus != NULL);
     
//    m_orient *= System::instance()->getCharacter()->getRotation();
    m_orient /= focus->getAbsOrient();
    //m_orient /= System::instance()->getCharacter()->getRotation();
//    m_orient /= WFMath::Quaternion(z_vector,  System::instance()->getCharacter()->getAngle());

    // Draw Sky box, requires the rotation to be done before any translation to keep the camera centered
    if (!select_mode) {
      m_renderer->store();
      m_renderer->applyQuaternion(m_orient);
      Environment::getInstance().renderSky();
      m_renderer->restore();
    }

    setCameraTransform();

    WFMath::Point<3> pos(0,0,0); // Initial camera positiona
    pos = focus->getAbsPos();
    assert (pos.isValid());

    m_renderer->getFrustum(m_frustum);
    // Setup main light sources
    m_renderer->applyLighting();

    Eris::View *view = avatar->getView();
    assert(view);
    WorldEntity *root = dynamic_cast<WorldEntity *>(view->getTopLevel());
    assert(root);


    // Build entity queues before rendering so entity lighting will be enabled.
    // Need to be careful we don't change our view matrices because of frustum
    //  culling

    assert(System::instance()->getCharacter());

    System::instance()->getCharacter()->updateLocals(false);
    m_render_queue.clear();
    m_message_list.clear();
    m_name_list.clear();

    buildQueues(root, 0, select_mode, m_render_queue, m_message_list, m_name_list, time_elapsed);


    if (select_mode ) {
      m_renderer->selectTerrainColour(root);
    }

    m_renderer->store();

    if (select_mode) {
      RenderSystem::getInstance().switchState(m_state_select);
    } else {
      RenderSystem::getInstance().switchState(m_state_terrain);
    }

    Environment::getInstance().renderTerrain(pos, select_mode);

    m_renderer->restore();

    m_renderer->drawQueue(m_render_queue, select_mode);
    if (!select_mode) {
      m_renderer->drawMessageQueue(m_message_list);
      if (m_show_names) {
        m_renderer->drawNameQueue(m_name_list);
      }
    }

    if (!select_mode ) {
      m_renderer->store();
      RenderSystem::getInstance().switchState(m_state_terrain);
      Environment::getInstance().renderSea();
      m_renderer->restore();

      //  Switch to 2D mode for rendering rain
      m_renderer->setViewMode(ORTHOGRAPHIC);

      RenderSystem::getInstance().switchState(m_state_weather);
      Environment::getInstance().renderWeather();
      // Switch back
      m_renderer->setViewMode(PERSPECTIVE);

      m_compass->update(cam->getRotation());
      m_compass->draw(m_renderer, select_mode);
    } 

  } else {
    m_renderer->drawSplashScreen();
  }
}


void Graphics::buildQueues(WorldEntity *we,
    int depth,
    bool select_mode,
    Render::QueueMap &render_queue,
    Render::MessageList &message_list,
    Render::MessageList &name_list,
    float time_elapsed)
{
  if (!we->isVisible()) return;

  Camera *cam = RenderSystem::getInstance().getCameraSystem()->getCurrentCamera();
  assert(cam != NULL);
  WorldEntity *self = dynamic_cast<WorldEntity*>(m_system->getClient()->getAvatar()->getEntity());

  assert(we->getType());
    
  SPtr<ObjectRecord> obj = ModelSystem::getInstance().getObjectRecord(we);
  assert (obj);

  // Setup lights as we go
  // TODO: This should be changed so that only the closest objects have light.
  if (we->type() == "fire") drawFire(we);
      
  // Loop through all models in list
  if (obj->draw_self) {
    if ((cam->getType() == Camera::CAMERA_FIRST) && (we == self)) { 
      /* first person, don't draw self */
    } else {
      drawObject(obj, select_mode, render_queue, message_list, name_list, time_elapsed);
    }
  }
  
  // Draw any contained objects
  if (obj->draw_members) {
    for (unsigned int i = 0; i < we->numContained(); ++i) {
          buildQueues(static_cast<WorldEntity*>(we->getContained(i)),
                      depth + 1,
                      select_mode,
                      render_queue,
                      message_list,
                      name_list,
                      time_elapsed);
    }
  } // of draw_members case
}

void Graphics::drawObject(SPtr<ObjectRecord> obj, 
                        bool select_mode,
                        Render::QueueMap &render_queue,
                        Render::MessageList &message_list,
                        Render::MessageList &name_list,
                        float time_elapsed) {
  assert(obj);

  WorldEntity *obj_we = dynamic_cast<WorldEntity*>(obj->entity.get());
  assert(obj_we); 

  // reject for drawing if object bbox is outside frustum   
  if (!Frustum::sphereInFrustum(m_frustum, obj->bbox, obj->position)) {
    obj_we->screenX() = -1;
    obj_we->screenY() = -1;
    return;
  }
    
  // Get world coord of object
  WFMath::Point<3> p = obj_we->getAbsPos();
  assert(p.isValid());
  // Transform world coord into camera coord
  WFMath::Vector<3> cam_pos(
    p.x() * m_modelview_matrix[0][0] 
     + p.y() * m_modelview_matrix[1][0] 
     + p.z() * m_modelview_matrix[2][0]
     + m_modelview_matrix[3][0],
    p.x() * m_modelview_matrix[0][1]
     + p.y() * m_modelview_matrix[1][1]
     + p.z() * m_modelview_matrix[2][1]
     + m_modelview_matrix[3][1],
    p.x() * m_modelview_matrix[0][2]
     + p.y() * m_modelview_matrix[1][2]
     + p.z() * m_modelview_matrix[2][2]
     + m_modelview_matrix[3][2]
  );

  // Calculate distance squared from camera
  float dist = cam_pos.sqrMag();

  ObjectRecord::ModelList::const_iterator I;
  ObjectRecord::ModelList::const_iterator Ibegin;
  ObjectRecord::ModelList::const_iterator Iend;
  // Choose low/medium/high quality queue based on distance from camera
  if (dist < m_high_dist) {
    Ibegin = obj->high_quality.begin();
    Iend = obj->high_quality.end();
  } else if (dist < m_medium_dist) {
    Ibegin = obj->medium_quality.begin();
    Iend = obj->medium_quality.end();
  } else {
    Ibegin = obj->low_quality.begin();
    Iend = obj->low_quality.end();
  }

  for (I = Ibegin; I != Iend; ++I) {
    // retrive or create the model and modelRecord as necessary
    SPtr<ModelRecord> modelRec = ModelSystem::getInstance().getModel(*I, obj_we);
    assert(modelRec);
    
    int state = select_mode ? modelRec->select_state : modelRec->state;
    
    if (state <= 0) continue; // bad state

    // Add to queue by state, then model record
    render_queue[state].push_back(Render::QueueItem(obj, modelRec));
    
    // Update Model
    if (!select_mode) { // Only needs to be done once a frame
        modelRec->model->update(time_elapsed);
        modelRec->model->setLastTime(System::instance()->getTimef());
    } 
    // Add attached objects to the render queues.
    if (obj->draw_attached || !obj_we->getAttachments().empty()) {
      WorldEntity::AttachmentMap::const_iterator it,
                                      end = obj_we->getAttachments().end();

      for (it = obj_we->getAttachments().begin(); it != end; ++it) {
        // retrieving the objectRecord also syncs it's pos with the WorldEntity
        if (!it->second) { continue; }
        Eris::Entity * ee = it->second.get();
        WorldEntity * we = dynamic_cast<WorldEntity*>(ee);
        assert(we!=0);

        std::string submodel = mapAttachSlotToSubmodel(it->first);
        PosAndOrient po = modelRec->model->getPositionForSubmodel(submodel);

//        we->setLocalOrient(we->getOrientation() * po.orient);
//        we->setLocalOrient(we->getAbsOrient() * po.orient);
        we->setLocalOrient(po.orient);
//        we->setLocalOrient(po.orient.inverse());
//        we->setLocalPos(we->getPredictedPos() + (po.pos.rotate(we->getOrientation().inverse())));
        we->setLocalPos(WFMath::Point<3>(po.pos.x(), po.pos.y(), po.pos.z()));

        SPtr<ObjectRecord> attached = ModelSystem::getInstance().getObjectRecord(we);
        assert (attached);
//
//        attached->orient = obj->orient * po.orient;
//        attached->position = obj->position +
//                                         (po.pos.rotate(obj->orient.inverse()));



        buildQueues(we,
                      2,
                      select_mode,
                      render_queue,
                      message_list,
                      name_list,
                      time_elapsed);

  //      drawObject(attached, select_mode, render_queue, message_list, name_list,
//
    //               time_elapsed);
      }
    }
  }
  
  // if rendering, add any messages
  if (!select_mode) {
    name_list.push_back(obj_we);
    if (obj_we->hasMessages() || obj_we->screenCoordsRequest() > 0) {
      message_list.push_back(obj_we);
    }
  } // of object models loop
}

void Graphics::drawFire(WorldEntity* we) {
  // Turn on light source
  m_fire.enabled = true;

  // Set position to entity posotion
  m_fire.position = we->getAbsPos();
  m_fire.position.z() += 0.5f; // Raise position off the ground a bit

  float status = we->valueOfAttr("status").asNum();
  // Clamp status range in case of bad values from server.
  if (status > 1.0f) status = 1.0f;
  else if (status < 0.0f) status = 0.0f;
 
  // TODO We need to use status to affect attenuation
  // But we need a good way to affect each component.
  // One component on its own gives too little, or too much light.

  // Add light to gl system
  m_fire.attenuation_constant =  1.0f;// - status;
  m_fire.attenuation_linear =  1.0f - status;
  m_fire.attenuation_quadratic =  0.0f;//1.0f - status;
  m_lm->applyLight(m_fire);
        
  // Disable as we don't need it again for now 
  m_fire.enabled = false;
}

std::string mapAttachSlotToSubmodel(const std::string& s)
{
    // temporary .. should be done via a config file
    if (s == "right_hand_wield") return "male_builder_right_hand";
    
    std::cerr << "no mapping from entity attachment slot '" << s << "'  to a submodel name" << std::endl;
    return s;
}

void Graphics::readConfig(varconf::Config &config) {
  varconf::Variable temp;

  // Read Distances for quality queues
//  if (config.findItem(GRAPHICS, KEY_low_dist)) {
//    temp =  config.getItem(GRAPHICS, KEY_low_dist);
//    m_low_dist = (!temp.is_double()) ? (DEFAULT_low_dist) : ((double)(temp));
//  } else {
//    m_low_dist = DEFAULT_low_dist;
//  } 
  if (config.findItem(GRAPHICS, KEY_medium_dist)) {
    temp =  config.getItem(GRAPHICS, KEY_medium_dist);
    m_medium_dist = (!temp.is_double()) ? (DEFAULT_medium_dist) : ((double)(temp));
  } else {
    m_medium_dist = DEFAULT_medium_dist;
  } 
  if (config.findItem(GRAPHICS, KEY_high_dist)) {
    temp =  config.getItem(GRAPHICS, KEY_high_dist);
    m_high_dist = (!temp.is_double()) ? (DEFAULT_high_dist) : ((double)(temp));
  } else {
    m_high_dist = DEFAULT_high_dist;
  } 
  // Read Fire properties 
  if (config.findItem(GRAPHICS, KEY_fire_ac)) {
    temp = config.getItem(GRAPHICS, KEY_fire_ac);
    m_fire.attenuation_constant = (!temp.is_double()) ? (DEFAULT_fire_ac) : ((double)(temp));
  } else {
    m_fire.attenuation_constant = DEFAULT_fire_ac;
  }
  if (config.findItem(GRAPHICS, KEY_fire_al)) {
    temp = config.getItem(GRAPHICS, KEY_fire_al);
    m_fire.attenuation_linear = (!temp.is_double()) ? (DEFAULT_fire_al) : ((double)(temp));
  } else {
    m_fire.attenuation_linear = DEFAULT_fire_al;
  }
  if (config.findItem(GRAPHICS, KEY_fire_aq)) {
    temp = config.getItem(GRAPHICS, KEY_fire_aq);
    m_fire.attenuation_quadratic = (!temp.is_double()) ? (DEFAULT_fire_aq) : ((double)(temp));
  } else {
    m_fire.attenuation_quadratic = DEFAULT_fire_aq;
  }

  if (config.findItem(GRAPHICS, KEY_fire_amb_red)) {
    temp = config.getItem(GRAPHICS, KEY_fire_amb_red);
    m_fire.ambient[0] = (!temp.is_double()) ? (DEFAULT_fire_amb_red) : ((double)(temp));
  } else {
    m_fire.ambient[0] = DEFAULT_fire_amb_red;
  }
  if (config.findItem(GRAPHICS, KEY_fire_amb_green)) {
    temp = config.getItem(GRAPHICS, KEY_fire_amb_green);
    m_fire.ambient[1] = (!temp.is_double()) ? (DEFAULT_fire_amb_green) : ((double)(temp));
  } else {
    m_fire.ambient[1] = DEFAULT_fire_amb_green;
  }
  if (config.findItem(GRAPHICS, KEY_fire_amb_blue)) {
    temp = config.getItem(GRAPHICS, KEY_fire_amb_blue);
    m_fire.ambient[2] = (!temp.is_double()) ? (DEFAULT_fire_amb_blue) : ((double)(temp));
  } else {
    m_fire.ambient[3] = DEFAULT_fire_amb_blue;
  }
  if (config.findItem(GRAPHICS, KEY_fire_amb_alpha)) {
    temp = config.getItem(GRAPHICS, KEY_fire_amb_alpha);
    m_fire.ambient[3] = (!temp.is_double()) ? (DEFAULT_fire_amb_alpha) : ((double)(temp));
  } else {
    m_fire.ambient[4] = DEFAULT_fire_amb_alpha;
  }


  if (config.findItem(GRAPHICS, KEY_fire_diff_red)) {
    temp = config.getItem(GRAPHICS, KEY_fire_diff_red);
    m_fire.diffuse[0] = (!temp.is_double()) ? (DEFAULT_fire_diff_red) : ((double)(temp));
  } else {
    m_fire.diffuse[0] = DEFAULT_fire_diff_red;
  }
  if (config.findItem(GRAPHICS, KEY_fire_diff_green)) {
    temp = config.getItem(GRAPHICS, KEY_fire_diff_green);
    m_fire.diffuse[1] = (!temp.is_double()) ? (DEFAULT_fire_diff_green) : ((double)(temp));
  } else {
    m_fire.diffuse[1] = DEFAULT_fire_diff_green;
  }
  if (config.findItem(GRAPHICS, KEY_fire_diff_blue)) {
    temp = config.getItem(GRAPHICS, KEY_fire_diff_blue);
    m_fire.diffuse[2] = (!temp.is_double()) ? (DEFAULT_fire_diff_blue) : ((double)(temp));
  } else {
    m_fire.diffuse[2] =  DEFAULT_fire_diff_blue;
  }
  if (config.findItem(GRAPHICS, KEY_fire_diff_alpha)) {
    temp = config.getItem(GRAPHICS, KEY_fire_diff_alpha);
    m_fire.diffuse[3] = (!temp.is_double()) ? (DEFAULT_fire_diff_alpha) : ((double)(temp));
  } else {
    m_fire.diffuse[3] = DEFAULT_fire_diff_alpha;
  }



  if (config.findItem(GRAPHICS, KEY_fire_spec_red)) {
    temp = config.getItem(GRAPHICS, KEY_fire_spec_red);
    m_fire.specular[0] = (!temp.is_double()) ? (DEFAULT_fire_spec_red) : ((double)(temp));
  } else {
    m_fire.specular[0] = DEFAULT_fire_spec_red;
  }
  if (config.findItem(GRAPHICS, KEY_fire_spec_green)) {
    temp = config.getItem(GRAPHICS, KEY_fire_spec_green);
    m_fire.specular[1] = (!temp.is_double()) ? (DEFAULT_fire_spec_green) : ((double)(temp));
  } else {
    m_fire.specular[1] = DEFAULT_fire_spec_green;
  }
  if (config.findItem(GRAPHICS, KEY_fire_spec_blue)) {
    temp = config.getItem(GRAPHICS, KEY_fire_spec_blue);
    m_fire.specular[2] = (!temp.is_double()) ? (DEFAULT_fire_spec_blue) : ((double)(temp));
  } else {
    m_fire.specular[2] = DEFAULT_fire_spec_blue;
  }
  if (config.findItem(GRAPHICS, KEY_fire_spec_alpha)) {
    temp = config.getItem(GRAPHICS, KEY_fire_spec_alpha);
    m_fire.specular[3] = (!temp.is_double()) ? (DEFAULT_fire_spec_alpha) : ((double)(temp));
  } else {
    m_fire.specular[3] = DEFAULT_fire_spec_alpha;
  }

}  

void Graphics::writeConfig(varconf::Config &config) {
  // Save queue quality settings 
//  config.setItem(GRAPHICS, KEY_low_dist, m_low_dist);
  config.setItem(GRAPHICS, KEY_medium_dist, m_medium_dist);
  config.setItem(GRAPHICS, KEY_high_dist, m_high_dist);
  // Save frame rate detail boundaries
  config.setItem(GRAPHICS, KEY_fire_ac, m_fire.attenuation_constant);
  config.setItem(GRAPHICS, KEY_fire_al, m_fire.attenuation_linear);
  config.setItem(GRAPHICS, KEY_fire_aq, m_fire.attenuation_quadratic);
  
  config.setItem(GRAPHICS, KEY_fire_amb_red, m_fire.ambient[Light::RED]);
  config.setItem(GRAPHICS, KEY_fire_amb_green, m_fire.ambient[Light::GREEN]);
  config.setItem(GRAPHICS, KEY_fire_amb_blue, m_fire.ambient[Light::BLUE]);
  config.setItem(GRAPHICS, KEY_fire_amb_alpha, m_fire.ambient[Light::ALPHA]);

  config.setItem(GRAPHICS, KEY_fire_diff_red, m_fire.diffuse[Light::RED]);
  config.setItem(GRAPHICS, KEY_fire_diff_green, m_fire.diffuse[Light::GREEN]);
  config.setItem(GRAPHICS, KEY_fire_diff_blue, m_fire.diffuse[Light::BLUE]);
  config.setItem(GRAPHICS, KEY_fire_diff_alpha, m_fire.diffuse[Light::ALPHA]);

  config.setItem(GRAPHICS, KEY_fire_spec_red, m_fire.specular[Light::RED]);
  config.setItem(GRAPHICS, KEY_fire_spec_green, m_fire.specular[Light::GREEN]);
  config.setItem(GRAPHICS, KEY_fire_spec_blue, m_fire.specular[Light::BLUE]);
  config.setItem(GRAPHICS, KEY_fire_spec_alpha, m_fire.specular[Light::ALPHA]);

}  

void Graphics::registerCommands(Console * console) {
  assert(console);

  console->registerCommand("invalidate", this);
  console->registerCommand("+show_names", this);
  console->registerCommand("-show_names", this);
  console->registerCommand("+select_mode", this);
  console->registerCommand("-select_mode", this);
  console->registerCommand("normalise_on", this);
  console->registerCommand("normalise_off", this);

}

void Graphics::runCommand(const std::string &command, const std::string &args) {
  if (command == "invalidate") {
    m_renderer->contextDestroyed(true);
    m_renderer->contextCreated();
  } else if (command == "+show_names") {
    m_show_names = true;
  } else if (command == "-show_names") {
    m_show_names = false;
  } else if (command == "+select_mode") {
    c_select = true;
  } else if (command == "-select_mode") {
    c_select = false;
  }
  else if (command == "normalise_on") glEnable(GL_NORMALIZE);
  else if (command == "normalise_off") glDisable(GL_NORMALIZE);

}

void Graphics::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  varconf::Variable temp;
  if (section != GRAPHICS) return;

//  if (key == KEY_low_dist) {
//    temp =  config.getItem(GRAPHICS, KEY_low_dist);
//    m_low_dist = ((double)(temp));
//  } 

//  else
 if (key == KEY_medium_dist) {
    temp =  config.getItem(GRAPHICS, KEY_medium_dist);
    m_medium_dist = ((double)(temp));
  } 

  else if (key == KEY_high_dist) {
    temp =  config.getItem(GRAPHICS, KEY_high_dist);
    m_high_dist = ((double)(temp));
  } 
}

} /* namespace Sear */
