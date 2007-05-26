// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall, University of Southampton

// $Id: Render.h,v 1.24 2007-05-26 18:49:10 simon Exp $

#ifndef SEAR_RENDER_H
#define SEAR_RENDER_H 1

#include <string>
#include <vector>
#include <map>

#include <wfmath/axisbox.h>

#include "common/Matrix.h"
#include "common/types.h"
#include "common/SPtr.h"

#include "loaders/StaticObject.h"
#include "loaders/DynamicObject.h"

#include "RenderTypes.h"

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
#define CAMERA (2)

class Render {

public:

typedef std::pair<SPtr<ObjectRecord>, SPtr<ModelRecord> > QueueItem;
typedef std::vector<QueueItem> Queue;
typedef std::map<int, Queue> QueueMap;
typedef std::vector<WorldEntity*> MessageList;
// New render queue types
typedef std::pair<Matrix, WorldEntity*> MatrixEntityItem;
typedef std::vector<MatrixEntityItem>  MatrixEntityList;
typedef std::map<std::string, MatrixEntityList> QueueMatrixMap;
typedef std::map<std::string, StateID> QueueStateMap;
typedef std::map<std::string, Queue> QueueOldMap;
typedef std::map<std::string, std::vector<SPtr<StaticObject> > > QueueStaticObjectMap;
typedef std::map<std::string, std::vector<SPtr<DynamicObject> > > QueueDynamicObjectMap;

  Render() :
    m_context_instantiation(-1),
    m_context_valid(false)
{
  }
  virtual ~Render() {}
   
//  virtual void renderMeshArrays(Mesh &mesh, unsigned int offset, bool multitexture)=0;
//  virtual void vboMesh(Mesh &mesh)=0;
//  virtual void cleanVBOMesh(Mesh &mesh) =0;

  virtual void init() = 0;
  virtual void shutdown() = 0;
  virtual bool isInitialised() const = 0;

  virtual int contextCreated() = 0;
  virtual void contextDestroyed(bool check) = 0;

  virtual void print(int x, int y, const char*, int set) = 0;
  virtual void print3D(const char*, int set) = 0;
  virtual void newLine() const = 0;

  virtual void getScreenCoords(int & x, int & y, double z_offset) const = 0;

  virtual void store() const = 0;
  virtual void restore() const = 0;
 
  virtual void beginFrame() = 0;
  virtual void endFrame(bool select_mode) = 0;
  virtual void drawSplashScreen() = 0;
  virtual void applyQuaternion(const WFMath::Quaternion & quaternion) const = 0;
  virtual void applyLighting() = 0;
  virtual void resetSelection() = 0;
  
  virtual float getLightLevel() const =0;
  
  virtual void buildColourSet() =0;
  virtual void drawTextRect(int, int, int, int, int) const = 0;
  virtual void setColour(float red, float blue , float green, float alpha) const = 0;
	  
  virtual void procEvent(int, int) = 0;
  virtual int getWindowWidth() const = 0;
  virtual int getWindowHeight() const = 0;

  virtual std::string getActiveID()const = 0;
  virtual WorldEntity * getActiveEntity()const = 0;

  virtual int axisBoxInFrustum(const WFMath::AxisBox<3> &) const = 0;
  virtual float distFromNear(float,float,float) const = 0;

  virtual void renderActiveName() =0;

  virtual void setupStates() =0;
  virtual void readConfig(varconf::Config &config) =0;
  virtual void writeConfig(varconf::Config &config) =0;

  virtual void translateObject(float x, float y, float z) const = 0;
  virtual void rotate(float angle, float x, float y, float z) const = 0;
  virtual void rotateObject(SPtr<ObjectRecord>, SPtr<ModelRecord>) const = 0;
  virtual void scaleObject(float scale) const = 0;
  virtual void setViewMode(int type) const = 0;
  virtual void setMaterial(float *ambient, float *diffuse, float *specular, float shininess, float *emissive) const = 0;
  virtual void renderArrays(unsigned int type, unsigned int offset, unsigned int number_of_points, Vertex_3 *vertex_data, Texel *texture_data, Normal *normal_data,bool) const = 0;
//  virtual void renderElements(unsigned int type, unsigned int number_of_points, int *faces_data, Vertex_3 *vertex_data, Texel *texture_data, Normal *normal_data,bool) =0;
  virtual void drawQueue(QueueMap &queue, bool select_mode) =0;

  virtual void drawQueue(const QueueStaticObjectMap &object_map,
                   const QueueMatrixMap &matrix_map,
                   const QueueStateMap &state_map,
                   bool select_mode) =0;

  virtual void drawQueue(const QueueDynamicObjectMap &object_map,
                   const QueueMatrixMap &matrix_map,
                   const QueueStateMap &state_map,
                   bool select_mode) =0;


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

  virtual void nextColour(WorldEntity*) = 0;

protected:
  int m_context_instantiation ;
  bool m_context_valid;
};
  
} /* namespace Sear */

#endif /* SEAR_RENDER_H */
