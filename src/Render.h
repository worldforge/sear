// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Render.h,v 1.24 2003-02-22 19:11:48 simon Exp $

#ifndef SEAR_RENDER_H
#define SEAR_RENDER_H 1

#include <SDL/SDL.h>
#include <string>
#include <list>
#include <map>
#include <utility>

#include <wfmath/axisbox.h>

//#include "ObjectLoader.h"

#define RENDER_FOV (45.0f)
//#define RENDER_FAR_CLIP (100.0f)
#define RENDER_FAR_CLIP (1000.0f)
#define RENDER_NEAR_CLIP (0.1f)

namespace Sear {

class Camera;
class Terrain;
class Sky;
class BoundBox;
class BillBoard;
class Impostor;
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

//typedef std::pair<ObjectRecord*, ModelRecord*> QueueItem;
typedef std::pair<ObjectRecord*, std::string> QueueItem;
//typedef std::pair<std::string, std::string> QueueItem;
typedef std::list<QueueItem> Queue;
typedef std::map<std::string, Queue> QueueMap;
typedef std::list<WorldEntity*> MessageList;

  Render() {
  }
  virtual ~Render() {}
  virtual void init() =0;
  virtual void initWindow(int width, int height) =0;
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
  
  virtual int requestTexture(const std::string &section, const std::string &texture, bool clamp = false) =0;
  virtual int requestMipMap(const std::string &section, const std::string &texture, bool clamp = false) =0;
  virtual void buildColourSet() =0;
  virtual unsigned int getTextureID(unsigned int texture_id)=0;
//  virtual void drawScene(const std::string &,bool, float) =0;
  virtual void drawTextRect(int, int, int, int, int) =0;
  virtual void stateChange(const std::string &state) =0;
  virtual void setColour(float red, float blue , float green, float alpha) =0;
	  
  virtual void procEvent(int, int) =0;
  virtual int getWindowWidth() =0;
  virtual int getWindowHeight() =0;

  virtual void switchTexture(int texture) =0;
  virtual void switchTextureID(unsigned int texture) =0;
  virtual void switchMultiTexture(int texture, int) =0;
  virtual void switchMultiTextureID(unsigned int texture, unsigned int) =0;
  virtual std::string getActiveID() =0;

  virtual int patchInFrustum(WFMath::AxisBox<3>) =0;
  virtual float distFromNear(float,float,float) =0;

  virtual void renderActiveName() =0;

//  virtual Camera* getCamera() =0;
//  virtual Terrain* getTerrain() =0;
//  virtual Sky* getSkyBox() =0;
 

  virtual void setupStates() =0;
  virtual void readConfig() =0;
  virtual void writeConfig() =0;

  virtual int requestTextureMask(const std::string &section, const std::string &texture, bool clamp = false) =0;
  virtual int requestMipMapMask(const std::string &section, const std::string &texture, bool clamp = false) =0;
  
  virtual void translateObject(float x, float y, float z) =0;
  virtual void rotate(float angle, float x, float y, float z) =0;
//  virtual void rotateObject(WorldEntity *we, int type) =0;
  virtual void rotateObject(ObjectRecord *, ModelRecord *) =0;
  virtual void scaleObject(float scale) =0;
  virtual void setViewMode(int type) =0;
  virtual void setMaterial(float *ambient, float *diffuse, float *specular, float shininess, float *emissive) =0;
  virtual void renderArrays(unsigned int type, unsigned int offset, unsigned int number_of_points, float *vertex_data, float *texture_data, float *normal_data,bool) =0;
  virtual void renderElements(unsigned int type, unsigned int number_of_points, int *faces_data, float *vertex_data, float *texture_data, float *normal_data,bool) =0;
  virtual unsigned int createTexture(unsigned int width, unsigned int height, unsigned int depth, unsigned char *data, bool clamp) =0;
  virtual void drawQueue(QueueMap &queue, bool select_mode, float time_elapsed) =0;
//  virtual void drawMessageQueue(QueueMap queue) =0;
  virtual void drawMessageQueue(MessageList &list) =0;

//  static WFMath::AxisBox<3> bboxCheck(WFMath::AxisBox<3> bbox) { return WFMath::AxisBox<3>();}

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
