// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: Render.h,v 1.16 2006-05-17 23:15:35 alriddoch Exp $

#ifndef SEAR_RENDER_H
#define SEAR_RENDER_H 1

#include <string>
#include <list>
#include <map>

#include <wfmath/axisbox.h>

#include "common/types.h"
#include "common/SPtr.h"

#define RENDER_FOV (45.0f)
#define RENDER_FAR_CLIP (1000.0f)
#define RENDER_NEAR_CLIP (0.1f)

namespace varconf {
  class Config;
}

namespace Sear {

class WorldEntity;
class ModelRecord;
class ObjectRecord;
class Mesh;

#define PERSPECTIVE (0)
#define ORTHOGRAPHIC (1)
#define ISOMETRIC (2)

class Render {

public:

typedef std::pair<SPtr<ObjectRecord>, SPtr<ModelRecord> > QueueItem;
typedef std::list<QueueItem> Queue;
typedef std::map<int, Queue> QueueMap;
typedef std::list<WorldEntity*> MessageList;

  Render() :
    m_context_instantiation(-1),
    m_context_valid(false)
{
  }
  virtual ~Render() {}
   
  virtual void renderMeshArrays(Mesh &mesh, unsigned int offset, bool multitexture)=0;
  virtual void vboMesh(Mesh &mesh)=0;
  virtual void cleanVBOMesh(Mesh &mesh) =0;

  virtual void init() =0;
  virtual void shutdown() =0;
  virtual bool isInitialised() const = 0;

  virtual int contextCreated() = 0;
  virtual void contextDestroyed(bool check) = 0;

  virtual void print(int x, int y, const char*, int set) =0;
  virtual void print3D(const char*, int set) =0;
  virtual void newLine() =0;

  virtual void getScreenCoords(int & x, int & y, double z_offset) = 0;

  virtual void store() =0;
  virtual void restore() =0;
 
  virtual void beginFrame() =0;
  virtual void endFrame(bool select_mode) =0; 
  virtual void drawSplashScreen() =0;
  virtual void applyQuaternion(const WFMath::Quaternion & quaternion) =0;
  virtual void applyLighting() =0;
  virtual void resetSelection() =0;
  
  virtual float getLightLevel() =0;
  
  virtual void buildColourSet() =0;
  virtual void drawTextRect(int, int, int, int, int) =0;
  virtual void setColour(float red, float blue , float green, float alpha) =0;
	  
  virtual void procEvent(int, int) = 0;
  virtual int getWindowWidth() const = 0;
  virtual int getWindowHeight() const = 0;

  virtual std::string getActiveID()const =0;
  virtual WorldEntity * getActiveEntity()const =0;

  virtual int axisBoxInFrustum(const WFMath::AxisBox<3> &) =0;
  virtual float distFromNear(float,float,float) =0;

  virtual void renderActiveName() =0;

  virtual void setupStates() =0;
  virtual void readConfig(varconf::Config &config) =0;
  virtual void writeConfig(varconf::Config &config) =0;

  virtual void translateObject(float x, float y, float z) =0;
  virtual void rotate(float angle, float x, float y, float z) =0;
  virtual void rotateObject(SPtr<ObjectRecord>, SPtr<ModelRecord>) =0;
  virtual void scaleObject(float scale) =0;
  virtual void setViewMode(int type) =0;
  virtual void setMaterial(float *ambient, float *diffuse, float *specular, float shininess, float *emissive) =0;
  virtual void renderArrays(unsigned int type, unsigned int offset, unsigned int number_of_points, Vertex_3 *vertex_data, Texel *texture_data, Normal *normal_data,bool) =0;
  virtual void renderElements(unsigned int type, unsigned int number_of_points, int *faces_data, Vertex_3 *vertex_data, Texel *texture_data, Normal *normal_data,bool) =0;
  virtual void drawQueue(QueueMap &queue, bool select_mode) =0;
  virtual void drawMessageQueue(MessageList &list) =0;
  virtual void drawNameQueue(MessageList &list) =0;

  virtual void applyCharacterLighting(float x, float y, float z) =0;
  virtual void getFrustum(float [6][4]) =0;
  virtual void getModelviewMatrix(float m[4][4]) = 0;
  
//  virtual void beginRecordList(unsigned int list) = 0;
//  virtual void endRecordList() = 0;
//  virtual unsigned int getNewList() = 0;
//  virtual void playList(unsigned int list) =0;
//  virtual void freeList(unsigned int list) = 0;

  virtual void selectTerrainColour(WorldEntity * we) = 0;

  void incrementContext() { m_context_instantiation++; }
  int currentContextNo() const { return m_context_instantiation; }
  bool contextValid() const { return m_context_valid; }

protected:
  int m_context_instantiation ;
  bool m_context_valid;;
};
  
} /* namespace Sear */

#endif /* SEAR_RENDER_H */
