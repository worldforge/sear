// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton


/*
 * This is the model viewer for Sear
 *
 * TODO
 * - Improve the UI it is *VERY* bad.
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

#include "src/Graphics.h"
#include "src/Render.h"

#include "common/Log.h"
#include "common/Utility.h"

Sear::Model *model = NULL;
Sear::Render *render = NULL;

bool _system_running = true;

Sear::ObjectProperties *op = NULL;

WFMath::Quaternion q = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

float camera_x = 0.0f;
float camera_y = 5.0f;
float camera_z = 0.0f;

float model_x = 0.0f;
float model_y = 0.0f;
float model_z = 0.0f;

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

void idle() {
  model->update(0.005f);
}

void handleEvents(const SDL_Event &event) {
  static int old_x = -1;
  static int old_y = -1;
  static bool mouse_down = false;
  switch (event.type) {
    case SDL_KEYDOWN: {
      if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q) exit(0);
      else if (event.key.keysym.sym == SDLK_t) model_x += 0.1f;
      else if (event.key.keysym.sym == SDLK_y) model_x -= 0.1f;
      else if (event.key.keysym.sym == SDLK_p) model_y -= 0.1f;
      else if (event.key.keysym.sym == SDLK_l) model_y += 0.1f;
      else if (event.key.keysym.sym == SDLK_z) model_z -= 0.1f;
      else if (event.key.keysym.sym == SDLK_x) model_z += 0.1f;
      else if (event.key.keysym.sym == SDLK_LEFT) camera_x += 0.1f;
      else if (event.key.keysym.sym == SDLK_RIGHT) camera_x -= 0.1f;
      else if (event.key.keysym.sym == SDLK_UP) camera_y -= 0.1f;
      else if (event.key.keysym.sym == SDLK_DOWN) camera_y += 0.1f;
      else if (event.key.keysym.sym == SDLK_PAGEUP) camera_z -= 0.1f;
      else if (event.key.keysym.sym == SDLK_PAGEDOWN) camera_z += 0.1f;
      else if (event.key.keysym.sym == SDLK_1) model->action("walk");
      else if (event.key.keysym.sym == SDLK_2) model->action("run");
      else if (event.key.keysym.sym == SDLK_3) model->action("wave");
      else if (event.key.keysym.sym == SDLK_4) model->action("funky");
      break;
    }
    case SDL_MOUSEBUTTONDOWN: mouse_down = true; break;
    case SDL_MOUSEBUTTONUP: mouse_down = false; break;
    case SDL_MOUSEMOTION: {
      if (!mouse_down) break;
      int x = event.button.x;
      int y = event.button.y;
      if (old_x == -1) { // then its the first time
        old_x = x;
        old_y = y;
        break;
      }
      int diff_x = old_x - x;
      int diff_y = old_y - y;

      q /= WFMath::Quaternion(WFMath::Vector<3>(0.0f, 0.0f, 1.0f), (float)diff_x / 5.0f);
      q /= WFMath::Quaternion(WFMath::Vector<3>(0.0f, 1.0f, 0.0f), (float)diff_y / 5.0f);
  
      old_x = x;
      old_y = y;
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
	
  glTranslatef(camera_x, camera_y, camera_z);

  render->stateChange("default");
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
  
  render->stateChange(op->state);
  // Don't want any fog or lighitng effects
  glDisable(GL_FOG);
  glDisable(GL_LIGHTING);
  glColor3f(1.0f, 1.0f, 1.0f);
     float rotation_matrix[4][4];
     Sear::QuatToMatrix(q, rotation_matrix); //Get the rotation matrix for base rotation
     glMultMatrixf(&rotation_matrix[0][0]); //Apply rotation matrix
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
  Sear::System *system = new Sear::System();
  system->init();
  system->createWindow(false);
  render = system->getGraphics()->getRender();
  Sear::ModelHandler *mh = system->getModelHandler();
  op = (Sear::ObjectProperties*)malloc(sizeof(Sear::ObjectProperties));
  model = mh->getModel(render, type, op);
            SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

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
  if (system) {
    system->shutdown();
    delete system;
    system = NULL;
  }
  
}
