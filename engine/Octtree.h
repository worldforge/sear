// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Octtree.h,v 1.1.2.1 2003-01-05 14:22:01 simon Exp $

#ifndef SEAR_ENGINE_OCTTREE_H
#define SEAR_ENGINE_OCTTREE_H 1

namespace Sear {

typedef enum {
  TOP_LEFT_FRONT = 0,
  TOP_LEFT_BACK,
  TOP_RIGHT_BACK,
  TOP_RIGHT_FRONT,
  BOTTOM_LEFT_FRONT,
  BOTTOM_LEFT_BACK,
  BOTTOM_RIGHT_BACK,
  BOTTOM_RIGHT_FRONT,
} NodeNames;
	
class OcttreeNode {
  bool free;
  bool divided;
  OcttreeNode *parent;
  OcttreeNode *children[8];
  WFMath::AxisBox<3> boundbox;
  void split();
  void merge();
};

class Octtree {
public:
  Octtree();
  ~Octtree();

  void init();
  void shutdown();

  void update();

  void setDimensions(WFMath::AxisBox<3> d) { _dimensions = d; }
  WFMath::AxisBox<3> getDimensions() const { return _dimensions; }
  
  void setDepth(unsigned int depth) { _recurs_depth = depth; }
  unsigned int getDepth() const { return _recurs_depth; }

 
  void addEntity(WorldEntity *);
  void removeEntity(WorldEntity *);

  EntityList getVisibleEntities();
  
private:
  static const unsigned int POOL_SIZE = 5000;

  
  bool _initialised;
  unsigned int _recurs_depth;
  float _variance;

  WFMath::AxisBox<3> _dimensions;
  
  unsigned int _pool_index;
  OcttreeNode[POOL_SIZE];
  
  OcttreeNode *allocateNode();
  void releaseNode(OcttreeNode *);

  void buildTree();
  void destroyTree();
  void updateTree();
};
	
} /* namespace Sear */

#endif /* SEAR_ENGINE_OCTTREE_H */
