// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $id: $

/*
 * This is the model viewer for Sear
 *
 * - Currently the design of Sear means we need the entire program to make this work!!!
 */ 

/*
 * CONTROLS
 * CAMERA is cursors and PAGE UP/DOWN
 * MODEL - hold down mouse button and move mouse at the same time
 * QUIT Escape of q
 * */


#include <iostream>
#include <string>

#include <png.h>

#include <sage/sage.h>
#include <sage/GL.h>
#include <sage/GLU.h>

#include <SDL/SDL.h>

#include <wfmath/quaternion.h>
#include <wfmath/vector.h>

#include "renderers/GL.h"
#include "renderers/CameraSystem.h"
#include "renderers/RenderSystem.h"

#include "loaders/Model.h"
#include "loaders/ModelLoader.h"
#include "loaders/ModelRecord.h"
#include "loaders/ObjectRecord.h"
#include "loaders/ModelHandler.h"
#include "loaders/ObjectHandler.h"
#include "loaders/ModelSystem.h"
#include "src/WorldEntity.h"
#include "src/System.h"

#include "renderers/Camera.h"

#include "renderers/Graphics.h"
#include "renderers/Render.h"

#include "common/Log.h"
#include "common/Utility.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

Sear::Render *render = NULL;
Sear::System *sys = NULL;
Sear::Camera *_camera = NULL;

bool show_axis = true;
bool _system_running = true;
float x_angle = 0.0f;
float y_angle = 0.0f;

Sear::ObjectRecord *object_record = NULL;
std::string the_state = "";
std::string type = "";
void display();
float model_x = 0.0f;
float model_y = 0.0f;
float model_z = 0.0f;
int move_dir = 0;
int width, height;
bool black = true;
void dump(const std::string &filename);


int save_png(const std::string &filename, unsigned char* buffer,
             int width, int height) {
  FILE *fp = fopen(filename.c_str(),"wb");
	
  if (!fp) {
    fprintf(stderr, "Error opening %s for writing\n", filename.c_str());
    return 1;
  }
	
  png_structp write_png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
        NULL, NULL, NULL);

  png_infop info_ptr = png_create_info_struct(write_png);

  png_init_io(write_png, fp);

  info_ptr->width = width;
  info_ptr->height = height;
  info_ptr->rowbytes = width * 4;
  info_ptr->bit_depth = 8;
  info_ptr->interlace_type = 0;
  info_ptr->num_palette = 0;
  info_ptr->palette = NULL;
  info_ptr->valid = 0;
	
  info_ptr->sig_bit.red = 8;
  info_ptr->sig_bit.green = 8;
  info_ptr->sig_bit.blue = 8;
  info_ptr->sig_bit.alpha = 8;
  info_ptr->color_type = PNG_COLOR_TYPE_RGB_ALPHA;

  png_write_info(write_png, info_ptr);

  unsigned char **row_pointers =
    (unsigned char**) malloc( info_ptr->height*sizeof(unsigned char*));	

  row_pointers[0]=buffer;

  for(int i=1; i<info_ptr->height; i++ )
    row_pointers[i] = row_pointers[i-1] + info_ptr->rowbytes;

  png_write_image(write_png, row_pointers);

  png_write_end(write_png, info_ptr);

  if ( info_ptr ) free( info_ptr->palette );
  free( row_pointers );

  png_destroy_write_struct(&write_png, &info_ptr);
fclose(fp);
  return 0;
}



void reshape(int w, int h) {
  width = w;
  height = h;
  //Check for divide by 0
  if (height == 0) height = 1;
  glViewport(0, 0, width, height);
//  glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Colour used to clear window
  glClearDepth(1.0); // Enables Clearing Of The Depth Buffer
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); // Reset The Projection Matrix

  // Calculate The Aspect Ratio Of The Window
  gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, 0.1f, 1000.0f);
  glMatrixMode(GL_MODELVIEW);

}
void updateMove(float time) {
  model_z += time * (float)move_dir;
}

void idle() {
  static unsigned int last_time = 0;
  float time = (((float)(sys->getTime() - last_time)) / 1000.0f);
  for (Sear::ObjectRecord::ModelList::const_iterator I = object_record->high_quality.begin(); I != object_record->high_quality.end(); I++) {
    Sear::ModelRecord *model_record = Sear::ModelSystem::getInstance().getModelHandler()->getModel(render, object_record, *I);
    if (model_record) {
      Sear::Model *model = model_record->model;   
      if (model) model->update(time);
    }
  }
  _camera->updateCameraPos(time);
  updateMove(time);
  last_time = sys->getTime();
}

void move(int i) {
  move_dir += i;
}

void handleEvents(const SDL_Event &event) {
  //object_record = Sear::ModelSystem::getInstance().getObjectHandler()->getObjectRecord(type);
  Sear::ObjectRecord *model = object_record;
  static bool mouse_down = false;
  switch (event.type) {
    case SDL_KEYDOWN: {
      if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q) exit(0);
      else if (event.key.keysym.sym == SDLK_EQUALS) move(1);
      else if (event.key.keysym.sym == SDLK_MINUS) move(-1);
      else if (event.key.keysym.sym == SDLK_LEFT) _camera->rotate(-1);
      else if (event.key.keysym.sym == SDLK_RIGHT) _camera->rotate(1);
      else if (event.key.keysym.sym == SDLK_PAGEUP) _camera->elevate(1);
      else if (event.key.keysym.sym == SDLK_PAGEDOWN) _camera->elevate(-1);
      else if (event.key.keysym.sym == SDLK_UP) _camera->zoom(-1);
      else if (event.key.keysym.sym == SDLK_DOWN) _camera->zoom(1);

      else if (event.key.keysym.sym == SDLK_1) model->action("standing");
      else if (event.key.keysym.sym == SDLK_2) model->action("walking");
      else if (event.key.keysym.sym == SDLK_3) model->action("running");
      else if (event.key.keysym.sym == SDLK_4) model->action("animation_grip_axe");
      else if (event.key.keysym.sym == SDLK_5) model->action("animation_wave");
      else if (event.key.keysym.sym == SDLK_6) model->action("animation_shoot_arrow");
      else if (event.key.keysym.sym == SDLK_7) model->action("animation_take_axe");
      else if (event.key.keysym.sym == SDLK_8) model->action("blend_grip_axe");
      else if (event.key.keysym.sym == SDLK_9) model->action("animation_stow_axe");
      else if (event.key.keysym.sym == SDLK_0) model->action("blend_limp");
      else if (event.key.keysym.sym == SDLK_w) model->action("animation_funky");
      else if (event.key.keysym.sym == SDLK_e) model->action("change_set_default");
      else if (event.key.keysym.sym == SDLK_r) model->action("change_set_extra");
      else if (event.key.keysym.sym == SDLK_t) model->action("add_mesh_belt_axe");
      else if (event.key.keysym.sym == SDLK_y) model->action("add_mesh_hand_axe");
      else if (event.key.keysym.sym == SDLK_u) model->action("remove_mesh_belt_axe");
      else if (event.key.keysym.sym == SDLK_i) model->action("remove_mesh_hand_axe");
      else if (event.key.keysym.sym == SDLK_o) model->action("set_equip_axe");
      else if (event.key.keysym.sym == SDLK_p) model->action("take");
      else if (event.key.keysym.sym == SDLK_a) model->action("stow");
      else if (event.key.keysym.sym == SDLK_s) model->action("clear_cycles");

      else if (event.key.keysym.sym == SDLK_g) show_axis = !show_axis;
      else if (event.key.keysym.sym == SDLK_c) black = !black;
      else if (event.key.keysym.sym == SDLK_b) {
        bool c = show_axis;
        show_axis = false;
        float y = x_angle;
        display();
        dump(type + ".png");
        x_angle += 90.0f;
        display();
        
        dump(type + ".90.png");
        show_axis = c; x_angle = y;
      }
      break;
    }
    case SDL_KEYUP: {
      if (event.key.keysym.sym == SDLK_EQUALS) move(-1);
      else if (event.key.keysym.sym == SDLK_MINUS) move(1);
      else if (event.key.keysym.sym == SDLK_LEFT) _camera->rotate(1);
      else if (event.key.keysym.sym == SDLK_RIGHT) _camera->rotate(-1);
      else if (event.key.keysym.sym == SDLK_PAGEUP) _camera->elevate(-1);
      else if (event.key.keysym.sym == SDLK_PAGEDOWN) _camera->elevate(1);
      else if (event.key.keysym.sym == SDLK_UP) _camera->zoom(1);
      else if (event.key.keysym.sym == SDLK_DOWN) _camera->zoom(-1);
      break;
    }
    case SDL_MOUSEBUTTONDOWN: mouse_down = true; break;
    case SDL_MOUSEBUTTONUP: mouse_down = false; break;
    case SDL_MOUSEMOTION: {
      if (!mouse_down) break;
      x_angle += (float)event.motion.xrel / 3.0f;
      y_angle += (float)event.motion.yrel / 3.0f;
      break;
    }			  
  }
}

void display() {
  // Set up view port
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//  glLoadIdentity();
//  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  static float ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};
  static float diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
//  glLightfv(GL_LIGHT0, GL_SPECULAR, lights[LIGHT_CHARACTER].specular);

  render->beginFrame();
glEnable(GL_ALPHA_TEST);
  if (black) glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Colour used to clear window
  else glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  WFMath::Quaternion orient = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
  orient /= WFMath::Quaternion(WFMath::Vector<3>(1.0f, 0.0f, 0.0f), _camera->getElevation());
  orient /= WFMath::Quaternion(WFMath::Vector<3>(0.0f, 0.0f, 1.0f), _camera->getRotation());
  glTranslatef(0.0f, _camera->getDistance(), 0.0f);
  render->applyQuaternion(orient);
//  glTranslatef(camera_x, camera_y, camera_z);
  Sear::RenderSystem::getInstance().switchState(Sear::RenderSystem::getInstance().requestState("default"));
  if (show_axis) {
    glBegin(GL_LINES);
      glColor3f(1.0f, 0.0f, 0.0f);
      glVertex3f(-10.0f, 0.0f, 0.0f);
      glVertex3f(10.0f, 0.0f, 0.0f);
      glColor3f(0.0f, 1.0f, 0.0f);
      glVertex3f(0.0f, -10.0f, 0.0f);
      glVertex3f(0.0f, 10.0f, 0.0f);
      glColor3f(0.0f, 0.0f, 1.0f);
      glVertex3f(0.0f, 0.0f, -10.0f);
      glVertex3f(0.0f, 0.0f, 10.0f);
    glEnd();
  }
  object_record = Sear::ModelSystem::getInstance().getObjectHandler()->getObjectRecord(type);
  the_state = (std::string)Sear::ModelSystem::getInstance().getModelRecords().getItem(type, "state"); 
  // Don't want any fog or lighitng effects
  glColor3f(1.0f, 1.0f, 1.0f);

  glRotatef(y_angle, 1.0f, 0.0f, 0.0f);
  glRotatef(x_angle, 0.0f, 0.0f, 1.0f);
//     float rotation_matrix[4][4];
//     Sear::QuatToMatrix(q, rotation_matrix); //Get the rotation matrix for base rotation
//     glMultMatrixf(&rotation_matrix[0][0]); //Apply rotation matrix
  glTranslatef(model_x, model_y, model_z);
  glPushMatrix();

//  std::cout << object_record->low_quality.size() << std::endl;
//  std::cout << object_record->name << std::endl;
  for (Sear::ObjectRecord::ModelList::const_iterator I = object_record->high_quality.begin(); I != object_record->high_quality.end(); I++) {
    Sear::ModelRecord *model_record = Sear::ModelSystem::getInstance().getModelHandler()->getModel(render, object_record, *I);
//    if (model_record) {
//      _system_running = false;
//      return;
//    }
    Sear::Model *model = model_record->model;   
  the_state = model_record->state_name; 
    if (model) {
      glPushMatrix();
  Sear::RenderSystem::getInstance().switchState(Sear::RenderSystem::getInstance().requestState(the_state));
      glDisable(GL_FOG);
      float scale = model_record->scale;
    glScalef(scale, scale,scale);
      model->render(false);
      glPopMatrix();
    } else {
      std::cerr << "Error no model" << std::endl;
    }
  }
  glPopMatrix();

  render->endFrame(false);
}

void loop () {
  SDL_Event event;
  while (true) {
    idle();
    display();
    while (SDL_PollEvent(&event)  ) {
      handleEvents(event);
      // Stop processing events if we are quiting
      if (!_system_running) break;
    }
  }
}

int main(int argc, char** argv) {
  if (argc >1) {
    type = argv[1];
    
  } else {
    std::cerr << "No model type specified!" << std::endl << std::flush;

    exit(1);
  }
  sys = new Sear::System();
  sys->init(argc, argv);
width = sys->getWidth();
height = sys->getHeight();
//  sys->
//  sys->createWindow(false);
  render = Sear::RenderSystem::getInstance().getRenderer();
  _camera = Sear::RenderSystem::getInstance().getCameraSystem()->getCurrentCamera();
  _camera->setMinDistance(0.0f);

  object_record = Sear::ModelSystem::getInstance().getObjectHandler()->getObjectRecord(type);
//            SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  if (object_record) {
     loop();
  } else {
    printf("Object records is NULL\n");
  }
  

  if (sys) {
    sys->shutdown();
    delete sys;
    sys = NULL;
  }
  
}

void dump (const std::string &filename) {
glEnable(GL_ALPHA_TEST);
  unsigned char *buf = new unsigned char[width * height * 4];
  glReadPixels(0,0,width, height, GL_RGBA, GL_UNSIGNED_BYTE, buf);
printf("%d %d %d %d\n",buf[400*4], buf[400*4+1], buf[400*4+2], buf[400 * 4+3]);
//  SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(buf, width, height, 32, width * 4, 0x000000ff, 0x0000ff00, 0x00ff0000,0x000000ff );
//  SDL_SaveBMP(surface, filename.c_str());
  save_png(filename, buf, width, height);
//  SDL_FreeSurface(surface);
delete [] buf;
}
