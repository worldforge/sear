#ifndef SEAR_ENVIRONMENT_H
#define SEAR_EnVIRONMENT_H

#include <stdlib.h>

namespace Sear {
class TerrainRenderer;

class Environment {
  Environment() :
    m_terrain(NULL)
  {}

public:
  ~Environment() {
  }

  void init();
  void shutdown();

  static Environment &getInstance() {
    return instance;
  }

  float getHeight(float x, float y);
  void setBasePoint(int x, int y, float z);

private:
  static Environment instance;
  TerrainRenderer *m_terrain;

};

}

#endif
