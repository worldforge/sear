// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

//----------------------------------------------------------------------------//
// model.h                                                                    //
// Copyright (C) 2001 Bruno 'Beosil' Heidelberger                             //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef MODEL_H
#define MODEL_H

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

//#include "global.h"
#include <cal3d/cal3d.h>
#include <GL/gl.h>
#include <GL/glu.h>
//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class Model
{
// misc
public:
  static const int STATE_IDLE;
  static const int STATE_FANCY;
  static const int STATE_MOTION;

// member variables
protected:
  int m_state;
  CalCoreModel m_calCoreModel;
  CalModel m_calModel;
  int m_animationId[16];
  int m_animationCount;
  int m_meshId[32];
  int m_meshCount;
  GLuint m_textureId[32];
  int m_textureCount;
  float m_motionBlend[3];
  float m_renderScale;
  float m_lodLevel;

// constructors/destructor
public:
	Model();
	virtual ~Model();

// member functions
public:
  void executeAction(int action);
  float getLodLevel();
  void getMotionBlend(float *pMotionBlend);
  float getRenderScale();
  int getState();
  bool onInit(const std::string& strFilename);
  void onRender(bool, bool, bool);
  void onShutdown();
  void onUpdate(float elapsedSeconds);
  void setLodLevel(float lodLevel);
  void setMotionBlend(float *pMotionBlend, float delay);
  void setState(int state, float delay);

protected:
  GLuint loadTexture(const std::string& strFilename);
  void renderMesh(bool bWireframe, bool bLight, bool);
  void renderSkeleton();
};

#endif

//----------------------------------------------------------------------------//
