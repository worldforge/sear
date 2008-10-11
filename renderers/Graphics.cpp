// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2008 Simon Goodall, University of Southampton

// $Id: Graphics.cpp,v 1.74 2008-10-05 09:54:17 simon Exp $

#include <sigc++/object_slot.h>

#include <sage/sage.h>
#include <sage/GL.h>

#include <varconf/Config.h>
#include <Eris/Entity.h>
#include <Eris/Avatar.h>
#include <Eris/View.h>
#include <wfmath/quaternion.h>
#include <wfmath/vector.h>

//#include "common/Log.h"
#include "common/Utility.h"
#include "environment/Environment.h"
#include "src/Character.h"
#include "src/Console.h"
#include "loaders/ModelSystem.h"
#include "loaders/Model.h"
#include "loaders/ModelRecord.h"
#include "loaders/ObjectRecord.h"
//#include "loaders/ObjectHandler.h"
#include "src/System.h"
#include "src/WorldEntity.h"
#include "src/client.h"
#include "gui/Compass.h"
#include "guichan/Workarea.h"

#include "Graphics.h"
#include "Camera.h"
//#include "Sprite.h"
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


static const WFMath::Vector<3> x_vector(1.0f, 0.0f, 0.0f);
static const WFMath::Vector<3> y_vector(0.0f, 1.0f, 0.0f);
static const WFMath::Vector<3> z_vector(0.0f, 0.0f, 1.0f);
static const WFMath::Quaternion quaternion_by_90(z_vector, WFMath::Pi / 2.0f);

static bool c_select = false;

static const std::string SECTION_graphics = "graphics";

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

  static const float DEFAULT_fire_amb_red = 0.4f;
  static const float DEFAULT_fire_amb_green = 0.3f;
  static const float DEFAULT_fire_amb_blue = 0.1f;
  static const float DEFAULT_fire_amb_alpha = 0.0f;
  static const float DEFAULT_fire_diff_red = 1.0f;
  static const float DEFAULT_fire_diff_green = 0.7f;
  static const float DEFAULT_fire_diff_blue = 0.2f;
  static const float DEFAULT_fire_diff_alpha = 0.0f;
  static const float DEFAULT_fire_spec_red = 0.4f;
  static const float DEFAULT_fire_spec_green = 0.3f;
  static const float DEFAULT_fire_spec_blue = 0.1f;
  static const float DEFAULT_fire_spec_alpha = 0.0f;


//  static const float DEFAULT_low_dist    = 1000.0f;
  static const float DEFAULT_medium_dist = 9000.0f;
  static const float DEFAULT_high_dist   = 4500.0f; 

static const std::string TYPE_fire = "fire";

static const std::string CMD_invalidate = "invalidate";
static const std::string CMD_show_bbox_on = "+show_bbox";
static const std::string CMD_show_bbox_off = "-show_bbox";
static const std::string CMD_show_names_on = "+show_names";
static const std::string CMD_show_names_off = "-show_names";
static const std::string CMD_select_mode_on = "+select_mode";
static const std::string CMD_select_mode_off = "-select_mode";
static const std::string CMD_normalise_on = "normalise_on";
static const std::string CMD_normalise_off = "normalise_off";

namespace Sear {

Graphics::Graphics(System *system) :
  m_system(system),
  m_renderer(NULL),
  m_num_frames(0),
  m_frame_time(0),
  m_initialised(false),
  m_compass(NULL),
  m_show_names(false),
  m_show_bbox(false),
  m_medium_dist(DEFAULT_medium_dist),
  m_high_dist(DEFAULT_high_dist)
{
}

Graphics::~Graphics() {
  if (m_initialised) shutdown();
}

void Graphics::init() {
  assert (m_initialised == false);

  // Add callbeck to detect updated options
  m_system->getGeneral().sigsv.connect(sigc::mem_fun(this, &Graphics::varconf_callback));

  // Create the compass
  m_compass = std::auto_ptr<Compass>(new Compass(580.f, 50.f));
  m_compass->setup();

  // Create the LightManager    
  m_lm = std::auto_ptr<LightManager>(new LightManager());
  m_lm->init();

  // Store state record id numbers
  m_state_weather = RenderSystem::getInstance().requestState("weather");
  m_state_terrain = RenderSystem::getInstance().requestState("terrain");
  m_state_select  = RenderSystem::getInstance().requestState("select");
  m_state_cursor  = RenderSystem::getInstance().requestState("cursor");

  m_initialised = true;
}

void Graphics::shutdown() {
  assert(m_initialised == true);
 
  m_compass.release();
  m_lm.release();

  m_initialised = false;
}

void Graphics::drawScene(bool select_mode, float time_elapsed) {
  assert(m_renderer != NULL);

  m_renderer->resetSelection();

  // Update camera position
  RenderSystem::getInstance().getCameraSystem()->getCurrentCamera()->updateCameraPos(time_elapsed);

  // Tell environment stuff to update
  Environment::getInstance().update(time_elapsed);

  // Do necessary GL initialisation for the frame
  m_renderer->beginFrame();

  // Draw the world!
  drawWorld(select_mode, time_elapsed);

  if (!select_mode) { 
    Workarea * wa = m_system->getWorkarea();
    assert (wa != NULL);
    try {
      wa->draw();
    } catch (const gcn::Exception &e) {
      fprintf(stderr, "Caught Guichan Exception\n");
    }

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
      const std::string &fr = "Sear: " + string_fmt(m_frame_rate);
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

void Graphics::setCameraTransform() {
  if (!m_system->checkState(SYS_IN_WORLD)) {
    return;
  }

  // Get the current camera
  const Camera *cam = RenderSystem::getInstance().getCameraSystem()->getCurrentCamera();
  assert(cam != NULL);

  // Get the current focus entity
  const Eris::Avatar *avatar = m_system->getClient()->getAvatar();
  assert(avatar != NULL);

  //Get the player character entity
  WorldEntity *focus = dynamic_cast<WorldEntity *>(avatar->getEntity());
  assert(focus != NULL);

  // Calculate entity height so camera is always at the top of entity.
  float height = (focus->hasBBox())
       ? (focus->getBBox().highCorner().z() - focus->getBBox().lowCorner().z())
       : (1.0f);

  //height += 0.5f; // Make the camera be just above focus height.
  height *= 1.1f; // Make the camera be just above focus height. 10% of height

  const WFMath::Point<3> &pos = focus->getAbsPos();

  // Adjust height so camera doesn't go underneath the terrain
  // Find terrain height at camera position, and adjust as if camera is at
  // entity feet. (the - height part)

  // Create camera vector
  WFMath::Vector<3> cam_vec(0.0f, -cam->getDistance(), 0.0f);

  // Rotate vec by camera orientation
  cam_vec.rotate(cam->getOrientation().inverse());
  // Rotate by focused entity orientation
  cam_vec.rotate(focus->getAbsOrient());
  // Rotate by 90 degrees -- So 0 deg is east
  cam_vec.rotate(quaternion_by_90.inverse());
  // Calculate new camera position
  const WFMath::Point<3> &p2 = pos + cam_vec;
  // Get height for camera
  float terrain_z = Environment::getInstance().getHeight(p2.x(), p2.y());
  // Adjust height by distance below ground so camera does not go below ground
  if (p2.z() + height < terrain_z) {
    //height += (terrain_z - p2.z());
    // Clamp height to just above the terrain
    height = (terrain_z - p2.z()) + 0.5f;
  }

  if (cam->getType() == Camera::CAMERA_CHASE) {
    // Translate camera getDist() units away from the character. Allows closups
    // or large views.
    m_renderer->translateObject(0.0f, cam->getDistance(), 0.0f);
  }
  // Set the avatar's lighting
  m_renderer->applyCharacterLighting(0.5, 0, 0.5);

  m_renderer->applyQuaternion(m_orient.inverse());

  // Translate to accumulated position - Also adjust so origin is nearer head
  // level
  m_renderer->translateObject(-pos.x(), -pos.y(), -pos.z() - height); 

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

    // TODO we could set this only when the server updates the values...
    float visibility = Environment::getInstance().getVisibility();
    glFogf(GL_FOG_START, visibility / 2.0f);
    glFogf(GL_FOG_END, visibility);

    const Camera *cam = RenderSystem::getInstance().getCameraSystem()->getCurrentCamera();
    assert(cam != NULL);

    // Initial Camera rotation
    m_orient.identity();

    // Rotate by 90 degrees, WF 0 degrees is East
    m_orient *= quaternion_by_90;
 
    const Eris::Avatar *avatar = m_system->getClient()->getAvatar();
    assert(avatar != NULL);

    //Get the player character entity
    WorldEntity *focus = dynamic_cast<WorldEntity *>(avatar->getEntity());
    assert(focus != NULL);
   
    // Make sure position and orientation is up-to-date
    // This will get called later on anyway, but we need this information
    // now to calculate the correct camera position.
    std::list<WorldEntity*> entity_list;

    // Push each parent location to the start of the list
    // for ordered processing later
    WorldEntity *we_update = focus;
    while (we_update != 0) {
      entity_list.push_front(we_update);
      we_update = dynamic_cast<WorldEntity*>(we_update->getLocation());
    }
    //  Update each entity in turn
    while (entity_list.empty() == false) {
      WorldEntity *we = entity_list.front();
      we->updateAbsOrient();
      we->updateAbsPosition();
      entity_list.pop_front();
    }
 
    // Apply character orientation 
    const WFMath::Quaternion &focus_orient = focus->getAbsOrient().inverse();
    assert(focus_orient.isValid());
    m_orient *= focus_orient;

    // Apply camera rotations

    const WFMath::Quaternion &cam_orient = cam->getOrientation();
    assert(cam_orient.isValid());
    m_orient *= cam_orient;

    // Draw Sky box, requires the rotation to be done before any translation to
    // keep the camera centered
    if (!select_mode && cam->getType() != Camera::CAMERA_ISOMETRIC) {
      glPushMatrix();
      m_renderer->applyQuaternion(m_orient.inverse());
      Environment::getInstance().renderSky();
      glPopMatrix();
    }

    setCameraTransform();

   // Initial camera positiona
    const WFMath::Point<3> &pos = focus->getAbsPos();
    assert (pos.isValid());

    m_renderer->getFrustum(m_frustum);
    // Setup main light sources
    m_renderer->applyLighting();

    const Eris::View *view = avatar->getView();
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

    m_static_object_map.clear();
    m_dynamic_object_map.clear();
    m_matrix_map.clear();
    m_state_map.clear();

    buildQueues(root, 0, select_mode, m_render_queue, m_message_list, m_name_list, time_elapsed);

    if (select_mode ) {
      m_renderer->selectTerrainColour(root);
    }

    glPushMatrix();

    if (select_mode) {
      RenderSystem::getInstance().switchState(m_state_select);
    } else {
      RenderSystem::getInstance().switchState(m_state_terrain);
    }

    Environment::getInstance().renderTerrain(pos, select_mode);

    glPopMatrix();

    m_renderer->drawQueue(m_render_queue, select_mode);
    m_renderer->drawQueue(m_static_object_map, m_matrix_map, m_state_map, select_mode);
    m_renderer->drawQueue(m_dynamic_object_map, m_matrix_map, m_state_map, select_mode);

    if (!select_mode) {
      m_renderer->drawMessageQueue(m_message_list);
      if (m_show_names) {
        m_renderer->drawNameQueue(m_name_list);
      }
    }

    if (!select_mode ) {
      glPushMatrix();
      RenderSystem::getInstance().switchState(m_state_terrain);
      Environment::getInstance().renderSea();
      glPopMatrix();

      //  Switch to 2D mode for rendering rain
      m_renderer->setViewMode(ORTHOGRAPHIC);

      // Render weather effects
      RenderSystem::getInstance().switchState(m_state_weather);
      Environment::getInstance().renderWeather();

      // Switch back to 3D
      m_renderer->setViewMode(PERSPECTIVE);

      // Draw the compass
      // TODO: Make this part of the GUI?
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
  if (!we->isVisible() && !we->isFading()) return;

  // Is this a good place to do the update?
  we->updateAbsOrient();
  we->updateAbsPosition();

  const Camera *cam = RenderSystem::getInstance().getCameraSystem()->getCurrentCamera();
  assert(cam != NULL);

  assert(we->getType());
    
  SPtr<ObjectRecord> obj = ModelSystem::getInstance().getObjectRecord(we);
  assert (obj);

  // Setup lights as we go
  // TODO: This should be changed so that only the closest objects have light.
  if (we->type() == TYPE_fire) drawFire(we);
      
  // Loop through all models in list
  if (obj->draw_self) {
    WorldEntity *self = dynamic_cast<WorldEntity*>(m_system->getClient()->getAvatar()->getEntity());
    if ((cam->getType() == Camera::CAMERA_FIRST) && (we == self)) { 
      /* first person, don't draw self */
    } else {
      drawObject(obj, select_mode, render_queue, message_list, name_list, time_elapsed);
    }
  }
  
  // Draw any contained objects
  for (unsigned int i = 0; i < we->numContained(); ++i) {
    WorldEntity *wec = static_cast<WorldEntity*>(we->getContained(i));
    if (obj->draw_members || wec->type() == TYPE_fire) {
      buildQueues(wec,
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

  // TODO: The bounding box should be rotated and translated just like entity
  // position. However, we can just translate and ignore rotation as we are 
  // using a sphere test and not working on the bbox directly.


  // TODO, Objects without bbox will be rendered regardless.
  if (obj_we->hasBBox()) {
    // reject for drawing if object bbox is outside frustum   
    if (!Frustum::sphereInFrustum(m_frustum, obj_we->getBBox(), obj_we->getAbsPos(), obj_we->getAbsOrient())) {
      obj_we->screenX() = -1;
      obj_we->screenY() = -1;
      return;
    }
  }

  // Get world coord of object
  const WFMath::Point<3> &p = obj_we->getAbsPos();
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

  // Choose low/medium/high quality queue based on distance from camera
  ObjectRecord::QueueType qt;
  if (dist < m_high_dist) {
    qt = ObjectRecord::QUEUE_high;
  } else if (dist < m_medium_dist) {
    qt = ObjectRecord::QUEUE_medium;
  } else {
    qt = ObjectRecord::QUEUE_low;
  }

  ObjectRecord::ModelList::const_iterator I;
  ObjectRecord::ModelList::const_iterator Ibegin = obj->quality_queue[qt].begin();
  ObjectRecord::ModelList::const_iterator Iend = obj->quality_queue[qt].end();

  if (!select_mode) { // Only needs to be done once a frame
    obj_we->updateFade(time_elapsed);
  }

  // Here we can insert a wireframe model to show the bounding box
  // of an entity using the +show_bbox command.
  if (m_show_bbox && obj_we->hasBBox()) {
    // Make sure we don't redraw the attached objects
    bool da = obj->draw_attached;
    obj->draw_attached = false;
    drawObjectExt("generic_wireframe", obj, obj_we, select_mode, render_queue, message_list, name_list, time_elapsed, dist);
    obj->draw_attached = da;
  }

  // BUG: If there is more than one model to render, then each model will 
  //      try to add any attached meshes to the render queue. This means the 
  //      same entity can be rendered multiple times!
  for (I = Ibegin; I != Iend; ++I) {
    drawObjectExt(*I, obj, obj_we, select_mode, render_queue, message_list, name_list, time_elapsed, dist);
  }
 
  // if rendering, add any messages
  if (!select_mode) {
    name_list.push_back(obj_we);
    if (obj_we->hasMessages() || obj_we->screenCoordsRequest() > 0) {
      message_list.push_back(obj_we);
    }
  } // of object models loop
}

void Graphics::drawObjectExt(const std::string &model_id,
                        SPtr<ObjectRecord> obj,
                        WorldEntity *obj_we,
                        bool select_mode,
                        Render::QueueMap &render_queue,
                        Render::MessageList &message_list,
                        Render::MessageList &name_list,
                        float time_elapsed, float camera_dist) {

  // retrieve or create the model and modelRecord as necessary
  SPtr<ModelRecord> modelRec = ModelSystem::getInstance().getModel(model_id, obj_we);
  assert(modelRec);
 
  SPtr<Model> model = modelRec->model;
  assert(model);

  int state = select_mode ? modelRec->select_state : modelRec->state;
    
  if (state <= 0) return; // bad state

  // Update Model
  if (!select_mode) { // Only needs to be done once a frame
    // Reduce detail level according to camera distance. Must be called before
    // model->update.
    // TODO: This might need some better scaling
    model->setDetailLevel(1.0f / camera_dist);

    // If we are fading in/out, then we update here
    obj_we->updateFade(time_elapsed);

    // Update any animations with elapsed time
    modelRec->model->update(time_elapsed);

    // Update last used time to delay model unloading.
    modelRec->model->setLastTime(System::instance()->getTimef());
  } 

  bool has_static = model->hasStaticObjects();
  bool has_dynamic = model->hasDynamicObjects();

  if (has_static || has_dynamic) {
 
// Calculate Transform Matrix //////////////////////////////////////////////////

    // Cheat and use the opengl matrix.
    glPushMatrix();
    glLoadIdentity();
  
    // 1) Apply Object transforms
    const WFMath::Point<3> &pos = obj_we->getAbsPos();
    assert(pos.isValid());
    glTranslatef(pos.x(), pos.y(), pos.z() );
  
    m_renderer->rotateObject(obj, modelRec);
    
    // 2) Apply Model Transforms 
     
    // Scale Object
    float scale = modelRec->scale;

    // Do not perform scaling if it is to zero or has no effect
    if (scale != 0.0f && scale != 1.0f) glScalef(scale, scale, scale);
 
    if (modelRec->offset_x != 0.0f || modelRec->offset_y != 0.0f || modelRec->offset_z != 0.0f) {
      glTranslatef(modelRec->offset_x, modelRec->offset_y, modelRec->offset_z);
    }

    if (modelRec->rotate_z != 0.0f) { 
      glRotatef(modelRec->rotate_z, 0.0f, 0.0f, 1.0f);
    }

    // 3) Apply final scaling once model is in place

    // Scale model by all bounding box axis
    if (modelRec->scale_bbox && obj_we->hasBBox()) { 
      const WFMath::AxisBox<3> &bbox = obj_we->getBBox();
      float x_scale = bbox.highCorner().x() - bbox.lowCorner().x();
      float y_scale = bbox.highCorner().y() - bbox.lowCorner().y();
      float z_scale = bbox.highCorner().z() - bbox.lowCorner().z();

      glScalef(x_scale, y_scale, z_scale);
    }

    // Scale model by bounding box height
    else if (modelRec->scaleByHeight && obj_we->hasBBox()) {
      const WFMath::AxisBox<3> &bbox = obj_we->getBBox();
      float z_scale = fabs(bbox.highCorner().z() - bbox.lowCorner().z());
      glScalef(z_scale, z_scale, z_scale);
    }

    float m[4][4];
    glGetFloatv(GL_MODELVIEW_MATRIX, &m[0][0]);

     // Restore matrix
    glPopMatrix();

    Matrix mx;
    mx.setMatrix(m);

////////////////////////////////////////////////////////////////////////////////

    // We need a different key depending on whether it's a static or dynamic object
    const std::string &key = (has_static) ? ("static_" + modelRec->id) : ("dynamic_" + obj_we->getId() + modelRec->id);

    m_matrix_map[key].push_back(Render::MatrixEntityItem(mx, obj_we));
    if (m_state_map.find(key) == m_state_map.end()) {
      m_state_map[key] = state;
      if (has_static)  m_static_object_map[key]  = model->getStaticObjects();
      if (has_dynamic) m_dynamic_object_map[key] = model->getDynamicObjects();
    }
  } else {
    // We still get here through the wireframe model
    // We also get here through the AreaModel and NullModel, but these can be ignored
    // as they have no meshes to render in the first place

    // Add to queue by state, then model record
    render_queue[state].push_back(Render::QueueItem(obj, modelRec));
  }

  // Add attached objects to the render queues.
  if (obj->draw_attached && !obj_we->getAttachments().empty()) {
    WorldEntity::AttachmentMap::const_iterator it,
                                      end = obj_we->getAttachments().end();

    for (it = obj_we->getAttachments().begin(); it != end; ++it) {
      // retrieving the objectRecord also syncs it's pos with the WorldEntity
      if (!it->second) { continue; }
      Eris::Entity *ee = it->second.get();
      WorldEntity *we = dynamic_cast<WorldEntity*>(ee);
      assert(we != 0);

      PosAndOrient po = modelRec->model->getPositionForSubmodel(it->first);

      we->setLocalOrient(po.orient);
      we->updateAbsOrient();

      if (modelRec->scale_bbox && obj_we->hasBBox()) {
        const WFMath::AxisBox<3> &bbox = obj_we->getBBox();
        float x_scale = bbox.highCorner().x() - bbox.lowCorner().x();
        float y_scale = bbox.highCorner().y() - bbox.lowCorner().y();
        float z_scale = bbox.highCorner().z() - bbox.lowCorner().z();
        po.pos.x() *= x_scale;
        po.pos.y() *= y_scale;
        po.pos.z() *= z_scale;
      }
      // Scale model by bounding box height
      else if (modelRec->scaleByHeight && obj_we->hasBBox()) {
        const WFMath::AxisBox<3> &bbox = obj_we->getBBox();
        float scale = fabs(bbox.highCorner().z() - bbox.lowCorner().z());
        po.pos *= scale;
      }

      // Convert Vector<3> to a Point<3>
      we->setLocalPos(WFMath::Point<3>(po.pos.x(), po.pos.y(), po.pos.z()));
      we->updateAbsPosition();

      buildQueues(we,
                    2, // depth is not used, so no need to give a real value.
                    select_mode,
                    render_queue,
                    message_list,
                    name_list,
                    time_elapsed);
    }
  }
}


void Graphics::drawFire(WorldEntity* we) {
  // Turn on light source
  m_fire.enabled = true;

  // Set position to entity posotion
  m_fire.position = we->getAbsPos();
  m_fire.position.z() += 0.5f; // Raise position off the ground a bit

  float status = we->getStatus();
  // Clamp status range in case of bad values from server.
  if (status > 1.0f) status = 1.0f;
  else if (status < 0.0f) status = 0.0f;
 
  // TODO We need to use status to affect attenuation
  // But we need a good way to affect each component.
  // One component on its own gives too little, or too much light.

  // Add light to gl system
  m_fire.attenuation_constant = 1.0f - status + 0.01f ;//0.5f;// status ;//1.0f;
  m_fire.attenuation_linear = 0.05f;//  1.0f - status;
  m_fire.attenuation_quadratic =  0.01f;//1.0f - status;
  m_lm->applyLight(m_fire);

  // Disable as we don't need it again for now 
  m_fire.enabled = false;
}

void Graphics::readConfig(varconf::Config &config) {
  varconf::Variable temp;

  // Read Distances for quality queues
  // m_low_dist = readDoubleValue(config, SECTION_graphics, KEY_low_dist, DEFAULT_low_dist);
  m_medium_dist = readDoubleValue(config, SECTION_graphics, KEY_medium_dist, DEFAULT_medium_dist);
  m_high_dist = readDoubleValue(config, SECTION_graphics, KEY_high_dist, DEFAULT_high_dist);

  // Read Fire properties 
  m_fire.attenuation_constant = readDoubleValue(config, SECTION_graphics, KEY_fire_ac, DEFAULT_fire_ac);
  m_fire.attenuation_linear = readDoubleValue(config, SECTION_graphics, KEY_fire_al, DEFAULT_fire_al);
  m_fire.attenuation_quadratic = readDoubleValue(config, SECTION_graphics, KEY_fire_aq, DEFAULT_fire_aq);

  m_fire.ambient[0] = readDoubleValue(config, SECTION_graphics, KEY_fire_amb_red, DEFAULT_fire_amb_red);
  m_fire.ambient[1] = readDoubleValue(config, SECTION_graphics, KEY_fire_amb_green, DEFAULT_fire_amb_green);
  m_fire.ambient[2] = readDoubleValue(config, SECTION_graphics, KEY_fire_amb_blue, DEFAULT_fire_amb_blue);
  m_fire.ambient[3] = readDoubleValue(config, SECTION_graphics, KEY_fire_amb_alpha, DEFAULT_fire_amb_alpha);


  m_fire.diffuse[0] = readDoubleValue(config, SECTION_graphics, KEY_fire_diff_red, DEFAULT_fire_diff_red);
  m_fire.diffuse[1] = readDoubleValue(config, SECTION_graphics, KEY_fire_diff_green, DEFAULT_fire_diff_green);
  m_fire.diffuse[2] = readDoubleValue(config, SECTION_graphics, KEY_fire_diff_blue, DEFAULT_fire_diff_blue);
  m_fire.diffuse[3] = readDoubleValue(config, SECTION_graphics, KEY_fire_diff_alpha, DEFAULT_fire_diff_alpha);


  m_fire.specular[0] = readDoubleValue(config, SECTION_graphics, KEY_fire_spec_red, DEFAULT_fire_spec_red);
  m_fire.specular[1] = readDoubleValue(config, SECTION_graphics, KEY_fire_spec_green, DEFAULT_fire_spec_green);
  m_fire.specular[2] = readDoubleValue(config, SECTION_graphics, KEY_fire_spec_blue, DEFAULT_fire_spec_blue);
  m_fire.specular[3] = readDoubleValue(config, SECTION_graphics, KEY_fire_spec_alpha, DEFAULT_fire_spec_alpha);
}  

void Graphics::writeConfig(varconf::Config &config) {
  // Save queue quality settings 
//  config.setItem(SECTION_graphics, KEY_low_dist, m_low_dist);
  config.setItem(SECTION_graphics, KEY_medium_dist, m_medium_dist);
  config.setItem(SECTION_graphics, KEY_high_dist, m_high_dist);
  // Save frame rate detail boundaries
  config.setItem(SECTION_graphics, KEY_fire_ac, m_fire.attenuation_constant);
  config.setItem(SECTION_graphics, KEY_fire_al, m_fire.attenuation_linear);
  config.setItem(SECTION_graphics, KEY_fire_aq, m_fire.attenuation_quadratic);
  
  config.setItem(SECTION_graphics, KEY_fire_amb_red, m_fire.ambient[Light::RED]);
  config.setItem(SECTION_graphics, KEY_fire_amb_green, m_fire.ambient[Light::GREEN]);
  config.setItem(SECTION_graphics, KEY_fire_amb_blue, m_fire.ambient[Light::BLUE]);
  config.setItem(SECTION_graphics, KEY_fire_amb_alpha, m_fire.ambient[Light::ALPHA]);

  config.setItem(SECTION_graphics, KEY_fire_diff_red, m_fire.diffuse[Light::RED]);
  config.setItem(SECTION_graphics, KEY_fire_diff_green, m_fire.diffuse[Light::GREEN]);
  config.setItem(SECTION_graphics, KEY_fire_diff_blue, m_fire.diffuse[Light::BLUE]);
  config.setItem(SECTION_graphics, KEY_fire_diff_alpha, m_fire.diffuse[Light::ALPHA]);

  config.setItem(SECTION_graphics, KEY_fire_spec_red, m_fire.specular[Light::RED]);
  config.setItem(SECTION_graphics, KEY_fire_spec_green, m_fire.specular[Light::GREEN]);
  config.setItem(SECTION_graphics, KEY_fire_spec_blue, m_fire.specular[Light::BLUE]);
  config.setItem(SECTION_graphics, KEY_fire_spec_alpha, m_fire.specular[Light::ALPHA]);

}  

void Graphics::registerCommands(Console * console) {
  assert(console);

  console->registerCommand(CMD_invalidate, this);
  console->registerCommand(CMD_show_bbox_on, this);
  console->registerCommand(CMD_show_bbox_off, this);
  console->registerCommand(CMD_show_names_on, this);
  console->registerCommand(CMD_show_names_off, this);
  console->registerCommand(CMD_select_mode_on, this);
  console->registerCommand(CMD_select_mode_off, this);
  console->registerCommand(CMD_normalise_on, this);
  console->registerCommand(CMD_normalise_off, this);
}

void Graphics::runCommand(const std::string &command, const std::string &args) {
  if (command == CMD_invalidate) {
    m_renderer->contextDestroyed(true);
    m_renderer->contextCreated();
  } else if (command == CMD_show_bbox_on) {
    m_show_bbox = true;
  } else if (command == CMD_show_bbox_off) {
    m_show_bbox = false;
  } else if (command == CMD_show_names_on) {
    m_show_names = true;
  } else if (command == CMD_show_names_off) {
    m_show_names = false;
  } else if (command == CMD_select_mode_on) {
    c_select = true;
  } else if (command == CMD_select_mode_off) {
    c_select = false;
  }
  else if (command == CMD_normalise_on) glEnable(GL_NORMALIZE);
  else if (command == CMD_normalise_off) glDisable(GL_NORMALIZE);

}

void Graphics::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  varconf::Variable temp;
  if (section != SECTION_graphics) return;

//  if (key == KEY_low_dist) {
//    temp =  config.getItem(SECTION_graphics, KEY_low_dist);
//    m_low_dist = ((double)(temp));
//  } 

//  else
 if (key == KEY_medium_dist) {
    temp =  config.getItem(SECTION_graphics, KEY_medium_dist);
    m_medium_dist = ((double)(temp));
  } 

  else if (key == KEY_high_dist) {
    temp =  config.getItem(SECTION_graphics, KEY_high_dist);
    m_high_dist = ((double)(temp));
  } 
}

} /* namespace Sear */
