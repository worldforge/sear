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

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <wfmath/quaternion.h>
#include <wfmath/vector.h>

#include "renderers/GL.h"

#include "src/Model.h"
#include "src/ModelLoader.h"
#include "src/ModelHandler.h"
#include "src/ObjectLoader.h"
#include "src/WorldEntity.h"

#include "common/Log.h"
#include "common/Utility.h"





Sear::Model *model = NULL;
Sear::GL *render = NULL;

Sear::ObjectProperties *op = NULL;

WFMath::Quaternion q = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

float camera_x = 0.0f;
float camera_y = 0.0f;
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

void display() {
  // Set up view port
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

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

  glDisable(GL_FOG);
  glColor3f(1.0f, 1.0f, 1.0f);
     float rotation_matrix[4][4];
     Sear::QuatToMatrix(q, rotation_matrix); //Get the rotation matrix for base rotation
     glMultMatrixf(&rotation_matrix[0][0]); //Apply rotation matrix
  glPushMatrix();
  
  if(op && op->scale) glScalef(op->scale, op->scale, op->scale); 
    model->render(false);
  glPopMatrix();
  glutSwapBuffers();
  glutPostRedisplay();
}

void key(unsigned char key, int x, int y) {
  switch (key) {
    case 27: 
    case 'q': exit(0); break;
    case 't': model_x += 0.1f; break;
    case 'y': model_x -= 0.1f; break;
    case 'p': model_y -= 0.1f; break;
    case 'l': model_y += 0.1f; break;
    case 'z': model_z -= 0.1f; break;
    case 'x': model_z += 0.1f; break;
  }
}

void special(int key, int x, int y) {
  switch (key) {
//    case GLUT_KEY_ESC: exit(0); break;
    case GLUT_KEY_LEFT: camera_x += 0.1f; break;
    case GLUT_KEY_RIGHT: camera_x -= 0.1f; break;
    case GLUT_KEY_UP: camera_y -= 0.1f; break;
    case GLUT_KEY_DOWN: camera_y += 0.1f; break;
    case GLUT_KEY_PAGE_UP: camera_z -= 0.1f; break;
    case GLUT_KEY_PAGE_DOWN: camera_z += 0.1f; break;
  }
}

int old_x = -1;
int old_y = -1;

void motion (int x, int y) {
	
  if (old_x == -1) { // then its the first time
    old_x = x;
    old_y = y;
    return;
  }
  int diff_x = old_x - x;
  int diff_y = old_y - y;

  q /= WFMath::Quaternion(WFMath::Vector<3>(0.0f, 0.0f, 1.0f), (float)diff_x / 5.0f);
  q /= WFMath::Quaternion(WFMath::Vector<3>(0.0f, 1.0f, 0.0f), (float)diff_y / 5.0f);
  
  old_x = x;
  old_y = y;
}

int main(int argc, char** argv) {
    std::string type = "";
  if (argc >1) {
    type = argv[1];
    
  } else {
    std::cerr << "No model type specified!" << std::endl << std::flush;

    exit(1);
  }
	  
  glutInit(&argc, argv);
  glutInitWindowSize(640, 480);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow("Sear Model Viewer");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);
  glutKeyboardFunc(key);
  glutSpecialFunc(special);
  glutMotionFunc(motion);
  
  Sear::System *system = new Sear::System();
  system->init();
  render = new Sear::GL(system, NULL);
  render->init();
  Sear::ModelHandler *mh = system->getModelHandler();
//  Sear::StateLoader *sl = system->getStateLoader();
  op = (Sear::ObjectProperties*)malloc(sizeof(Sear::ObjectProperties));
//  Sear::WorldEntity *we;
//  we = new WorldEntity();
  model = mh->getModel(render, type, op);
  if (model) glutMainLoop(); 

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
