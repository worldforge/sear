// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _RENDER_H_
#define _RENDER_H_ 1

#include <SDL/SDL.h>
#include <string>
#include <list>
#include <map>

#include "ObjectLoader.h"
#include <wfmath/axisbox.h>

#define RENDER_FOV (45.0f)
//#define RENDER_FAR_CLIP (100.0f)
#define RENDER_FAR_CLIP (1000.0f)
#define RENDER_NEAR_CLIP (0.1f)

namespace Sear {

class Camera;
class Terrain;
class SkyBox;
class BoundBox;
class BillBoard;
class Impostor;
class WorldEntity;


typedef enum {
  RENDER_UNKNOWN = 0,
  RENDER_LIGHTING,
  RENDER_TEXTURES,
  RENDER_SHADOWS,
  RENDER_FPS,
  RENDER_STENCIL,
  RENDER_LAST_STATE
} RenderState;


#define FONT_TO_SKYBOX          (0)
#define SKYBOX_TO_TERRAIN       (1)
#define TERRAIN_TO_WIREFRAME    (2)
#define WIREFRAME_TO_CHARACTERS (3)
#define CHARACTERS_TO_MODELS    (4)
#define MODELS_TO_BILLBOARD     (5)
#define BILLBOARD_TO_FONT       (6)
#define FONT_TO_PANEL           (7)
#define PANEL_TO_FONT           (8)
#define LAST_CHANGE             (9)

class Render {

public:
typedef std::list<WorldEntity*> Queue;
typedef enum {
  SKYBOX = 0,
  TERRAIN,
  WIREFRAME,
  CHARACTERS,
  MODELS,
  BILLBOARD,
  PANEL,
  FONT,
  SELECT,
  HALO,
  LAST_STATE
} State;

  Render() {
    int i;
    for (i = 1; i < RENDER_LAST_STATE; _renderState[i++] = false);
  }
  virtual ~Render() {}
  virtual void init() {}
  virtual void initWindow(int width, int height) {}
  virtual void shutdown() {};

  virtual void print(int x, int y, const char*, int set) {}
  virtual void print3D(const char*, int set) {}
  virtual void newLine() {}

  virtual float getLightLevel() { return 1.0f; }
  
  virtual int requestTexture(const std::string&, bool clamp = false) { return 0;}
  virtual int requestMipMap(const std::string &, bool clamp = false) { return 0;}
  virtual void buildColourSet() {}
  virtual unsigned int getTextureID(int texture_id) { return 0;}
  virtual void drawScene(const std::string &,bool) {}
  virtual void drawTextRect(int, int, int, int, int) {}
  virtual void stateChange(const std::string &state) {}
  virtual void stateChange(State) {}
  virtual void setColour(float red, float blue , float green, float alpha) {}
	  
  virtual void procEvent(int, int) {}
  virtual int getWindowWidth() { return 0; }
  virtual int getWindowHeight() { return 0; }

  virtual void switchTexture(int texture) {}
  virtual void switchTextureID(unsigned int texture) {}
  virtual std::string getActiveID() { return "";}

  virtual int patchInFrustum(WFMath::AxisBox<3>) { return 0;}
  virtual float distFromNear(float,float,float) { return 0.0f;}

  virtual void setCallyMotion(float, float, float) {}
  virtual void setCallyState(int) {}
  virtual void executeCallyAction(int) {}
  virtual Camera* getCamera() { return NULL; }
  virtual Terrain* getTerrain() { return NULL; }
  virtual SkyBox* getSkyBox() { return NULL; }
  virtual void pushMessage(const std::string &msg) {}
 
  void setState(RenderState rs, bool state) { _renderState[rs] = state; }
  bool checkState(RenderState rs) { return _renderState[rs]; }

  virtual void processObjectProperties(ObjectProperties *){}
  virtual void checkModelStatus(const std::string &) {}
  virtual void setModelInUse(const std::string &, bool) {} 
  virtual void setupStates() {}
  virtual void readConfig() {}
  virtual void writeConfig() {}
  virtual void readComponentConfig() {}
  virtual void writeComponentConfig() {}
protected:
  bool _renderState[RENDER_LAST_STATE];
private:
  
public:

  virtual int requestTextureMask(const std::string&, bool clamp = false) { return 0;}
  virtual void createTextureMask(SDL_Surface*, unsigned int, bool) {}
  
  virtual void translateObject(float x, float y, float z) {}
  virtual void rotateObject(WorldEntity *we, int type) {}
  virtual void setViewMode(int type) {}
  virtual void setMaterial(float *ambient, float *diffuse, float *specular, float shininess, float *emissive) {}
  virtual void renderArrays(unsigned int type, unsigned int number_of_points, float *vertex_data, float *texture_data, float *normal_data) {}
  virtual void renderElements(unsigned int type, unsigned int number_of_points, int *faces_data, float *vertex_data, float *texture_data, float *normal_data) {}
  virtual unsigned int createTexture(unsigned int width, unsigned int height, unsigned int depth, unsigned char *data, bool clamp) { return 0;}
  virtual void drawQueue(std::map<std::string, Queue> queue, bool select_mode, float time_elapsed) {}

//  static WFMath::AxisBox<3> bboxCheck(WFMath::AxisBox<3> bbox) { return WFMath::AxisBox<3>();}

  };  

} /* namespace Sear */

#endif /* _RENDER_H_ */
