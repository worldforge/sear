// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton


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


#include <iostream.h>
#include <string>

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <wfmath/quaternion.h>
#include <wfmath/vector.h>

#include "renderers/GL.h"

#include "src/Model.h"
#include "src/ModelLoader.h"
#include "src/ModelHandler.h"
#include "src/ObjectLoader.h"
#include "src/WorldEntity.h"

#include "src/Camera.h"

#include "src/Graphics.h"
#include "src/Render.h"

#include "common/Log.h"
#include "common/Utility.h"

Sear::Model *model = NULL;
Sear::Render *render = NULL;
Sear::System *sys = NULL;
Sear::Camera *_camera = NULL;

bool show_axis = true;
bool _system_running = true;
float x_angle = 0.0f;
float y_angle = 0.0f;

Sear::ObjectProperties *op = NULL;

float model_x = 0.0f;
float model_y = 0.0f;
float model_z = 0.0f;
int move_dir = 0;

void reshape(int width, int height) {
  //Check for divide by 0
  if (height == 0) height = 1;
  glViewport(0, 0, width, height);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Colour used to clear window
  glClearDepth(1.0); // Enables Clearing Of The Depth Buffer
  glClear(GL_DEPTH_BUFFER_BIT);

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
  model->update(time);
  _camera->updateCameraPos(time);
  updateMove(time);
  last_time = sys->getTime();
}

void move(int i) {
  move_dir += i;
}


void handleEvents(const SDL_Event &event) {
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
      else if (event.key.keysym.sym == SDLK_1) model->action("idle");
      else if (event.key.keysym.sym == SDLK_2) model->action("walk");
      else if (event.key.keysym.sym == SDLK_3) model->action("run");
      else if (event.key.keysym.sym == SDLK_4) model->action("strut");
      else if (event.key.keysym.sym == SDLK_5) model->action("wave");
      else if (event.key.keysym.sym == SDLK_6) model->action("shoot_arrow");
      else if (event.key.keysym.sym == SDLK_7) model->action("funky");
      else if (event.key.keysym.sym == SDLK_g) show_axis = !show_axis;
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

  render->beginFrame();
  glClear(GL_COLOR_BUFFER_BIT);
  WFMath::Quaternion orient = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
  orient /= WFMath::Quaternion(WFMath::Vector<3>(1.0f, 0.0f, 0.0f), _camera->getElevation());
  orient /= WFMath::Quaternion(WFMath::Vector<3>(0.0f, 0.0f, 1.0f), _camera->getRotation());
  glTranslatef(0.0f, _camera->getDistance(), 0.0f);
  render->applyQuaternion(orient);
//  glTranslatef(camera_x, camera_y, camera_z);

  render->stateChange("default");
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
  
  render->stateChange(op->state);
  // Don't want any fog or lighitng effects
  glDisable(GL_FOG);
  glDisable(GL_LIGHTING);
  glColor3f(1.0f, 1.0f, 1.0f);

  glRotatef(y_angle, 1.0f, 0.0f, 0.0f);
  glRotatef(x_angle, 0.0f, 0.0f, 1.0f);
//     float rotation_matrix[4][4];
//     Sear::QuatToMatrix(q, rotation_matrix); //Get the rotation matrix for base rotation
//     glMultMatrixf(&rotation_matrix[0][0]); //Apply rotation matrix
  glTranslatef(model_x, model_y, model_z);
  glPushMatrix();
  
  if(op && op->scale) glScalef(op->scale, op->scale, op->scale); 
    model->render(false);
  glPopMatrix();
  glFlush();
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
    std::string type = "";
  if (argc >1) {
    type = argv[1];
    
  } else {
    std::cerr << "No model type specified!" << std::endl << std::flush;

    exit(1);
  }
  sys = new Sear::System();
  sys->init();
  sys->createWindow(false);
  render = sys->getGraphics()->getRender();
  _camera = sys->getGraphics()->getCamera();
  Sear::ModelHandler *mh = sys->getModelHandler();
  op = (Sear::ObjectProperties*)malloc(sizeof(Sear::ObjectProperties));
  model = mh->getModel(render, type, op);
//            SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  if (model) loop();

  if (model) {
    model->shutdown();
    delete model;
    model = NULL;
  }
  if (op) free (op);
  if (mh) {
    mh->shutdown();	  
    delete mh;
    mh = NULL;
  }
  if (sys) {
    sys->shutdown();
    delete sys;
    sys = NULL;
  }
  
}
