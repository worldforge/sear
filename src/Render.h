// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: Render.h,v 1.35 2004-04-26 20:26:50 simon Exp $

#ifndef SEAR_RENDER_H
#define SEAR_RENDER_H 1

#include <SDL/SDL.h>
#include <string>
#include <list>
#include <map>
#include <utility>

#include <wfmath/axisbox.h>

#include "common/types.h"

#define RENDER_FOV (45.0f)
#define RENDER_FAR_CLIP (1000.0f)
#define RENDER_NEAR_CLIP (0.1f)

namespace Sear {

class WorldEntity;
class ModelRecord;
class ObjectRecord;

#define PERSPECTIVE (0)
#define ORTHOGRAPHIC (1)
#define ISOMETRIC (2)

class Render {

public:
typedef enum {
  RENDER_UNKNOWN = 0,
  RENDER_LIGHTING,
  RENDER_TEXTURES,
  RENDER_SHADOWS,
  RENDER_FPS,
  RENDER_STENCIL,
  RENDER_LAST_STATE
} RenderState;

typedef std::pair<ObjectRecord*, std::string> QueueItem;
typedef std::list<QueueItem> Queue;
typedef std::map<int, Queue> QueueMap;
typedef std::list<WorldEntity*> MessageList;

  Render() {
  }
  virtual ~Render() {}
   
  virtual void renderMeshArrays(Mesh &mesh, unsigned int offset, bool multitexture)=0;
  virtual void vboMesh(Mesh &mesh)=0;
  virtual void cleanVBOMesh(Mesh &mesh) =0;

  virtual void init() =0;
  virtual void shutdown() =0;

  virtual void print(int x, int y, const char*, int set) =0;
  virtual void print3D(const char*, int set) =0;
  virtual void newLine() =0;

  virtual void store() =0;
  virtual void restore() =0;
 
  virtual void beginFrame() =0;
  virtual void endFrame(bool select_mode) =0; 
  virtual void drawFPS(float fps) =0;
  virtual void drawSplashScreen() =0;
  virtual void applyQuaternion(WFMath::Quaternion quaternion) =0;
  virtual void applyLighting() =0;
  virtual void resetSelection() =0;
  
  virtual float getLightLevel() =0;
  
  virtual void buildColourSet() =0;
  virtual void drawTextRect(int, int, int, int, int) =0;
  virtual void setColour(float red, float blue , float green, float alpha) =0;
	  
  virtual void procEvent(int, int) =0;
  virtual int getWindowWidth() =0;
  virtual int getWindowHeight() =0;

  virtual std::string getActiveID() =0;

  virtual int patchInFrustum(WFMath::AxisBox<3>) =0;
  virtual float distFromNear(float,float,float) =0;

  virtual void renderActiveName() =0;

  virtual void setupStates() =0;
  virtual void readConfig() =0;
  virtual void writeConfig() =0;

  virtual void translateObject(float x, float y, float z) =0;
  virtual void rotate(float angle, float x, float y, float z) =0;
  virtual void rotateObject(ObjectRecord *, ModelRecord *) =0;
  virtual void scaleObject(float scale) =0;
  virtual void setViewMode(int type) =0;
  virtual void setMaterial(float *ambient, float *diffuse, float *specular, float shininess, float *emissive) =0;
  virtual void renderArrays(unsigned int type, unsigned int offset, unsigned int number_of_points, Vertex_3 *vertex_data, Texel *texture_data, Normal *normal_data,bool) =0;
  virtual void renderElements(unsigned int type, unsigned int number_of_points, int *faces_data, Vertex_3 *vertex_data, Texel *texture_data, Normal *normal_data,bool) =0;
  virtual void drawQueue(QueueMap &queue, bool select_mode, float time_elapsed) =0;
  virtual void drawMessageQueue(MessageList &list) =0;

  void setState(RenderState rs, bool state) { _renderState[rs] = state; }
  bool checkState(RenderState rs) { return _renderState[rs]; }

  virtual void applyCharacterLighting(float x, float y, float z) =0;
  virtual void getFrustum(float [6][4]) =0;

  virtual void beginRecordList(unsigned int list) = 0;
  virtual void endRecordList() = 0;
  virtual unsigned int getNewList() = 0;
  virtual void playList(unsigned int list) =0;
  virtual void freeList(unsigned int list) = 0;
  
protected:
  bool  _renderState[RENDER_LAST_STATE];

};
  
} /* namespace Sear */

#endif /* SEAR_RENDER_H */
