// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Patch.h,v 1.9 2002-12-14 14:46:36 simon Exp $

// Code based upon ROAM Simplistic Implementation by Bryan Turner bryan.turner@pobox.com
#ifndef SEAR_PATCH_H
#define SEAR_PATCH_H 1

// Depth of variance tree: should be near SQRT(PATCH_SIZE) + 1
//#define VARIANCE_DEPTH (6)
#define VARIANCE_DEPTH (9)
#include "common/Utility.h"
//#include <queue>

namespace Sear {

// Predefines...
class ROAM;
class Landscape;
class Render;
//
// TriTreeNode Struct
// Store the triangle tree data, but no coordinates!
//
struct TriTreeNode {
  TriTreeNode *LeftChild;
  TriTreeNode *RightChild;
  TriTreeNode *BaseNeighbor;
  TriTreeNode *LeftNeighbor;
  TriTreeNode *RightNeighbor;
  Landscape *l;
//  float priority;
//  bool operator < (TriTreeNode *tri){ return (this->priority < tri->priority); };
  TriTreeNode() :
    LeftChild(NULL),
    RightChild(NULL),
    BaseNeighbor(NULL),
    LeftNeighbor(NULL),
    RightNeighbor(NULL),
    l(NULL)
  {}
};

//typedef pair<TriTreeNode*, TriTreeNode*> diamond;
class Patch {
protected:
  float *m_HeightMap;
  int m_WorldX, m_WorldY; // World coordinate offset of this patch.
  
  float m_VarianceLeft[1<<(VARIANCE_DEPTH)];	// Left variance tree
  float m_VarianceRight[1<<(VARIANCE_DEPTH)];	// Right variance tree
  float *m_CurrentVariance; // Which varience we are currently using. [Only valid during the Tessellate and ComputeVariance passes]
  unsigned char m_VarianceDirty; // Does the Varience Tree need to be recalculated for this Patch?
  unsigned char m_isVisible; // Is this patch visible in the current frame?

  TriTreeNode m_BaseLeft; // Left base triangle tree node
  TriTreeNode m_BaseRight; // Right base triangle tree node

public:
  Patch();
  Patch(Render *renderer, ROAM *terrain, Landscape *landscape);
  ~Patch();
  // Some encapsulation functions & extras
  TriTreeNode *GetBaseLeft()  { return &m_BaseLeft; }
  TriTreeNode *GetBaseRight() { return &m_BaseRight; }
  char isDirty()     { return m_VarianceDirty; }
  int  isVisibile( ) { return m_isVisible; }
  void SetVisibility();// int eyeX, int eyeY, int leftX, int leftY, int rightX, int rightY );

  // The static half of the Patch Class
  void Init( int heightX, int heightY, int worldX, int worldY, float *hMap );
  void Reset();
  void Tessellate();
  void render();
  void renderWater();
  void ComputeVariance();

  // The recursive half of the Patch Class
  void Split( TriTreeNode *tri);
  void RecursTessellate( TriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY, int node );
  void RecursRender( TriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY );
  void RecursRenderWater( TriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY );
  float RecursComputeVariance(int leftX,  int leftY, float leftZ, int rightX, int rightY, float rightZ, int apexX,  int apexY, float apexZ, int node);

//priority_queue<TriTreeNode*> split_queue;// = priority_queue<TriTreeNode*>();
//priority_queue<diamond> merge_queue;// = priority_queue<diamond>();
  
  Render *_renderer;
  ROAM *_terrain;
  Landscape *_landscape;


//  static const int n = (const int)(log(25)/log(2));
//  static const int rdepth = 2*n -1;
  static const int array_size = 3 * 2 * (int)((200.0f / 8.0f) * (200.0f / 8.0f));
  int v_counter, n_counter, t_counter;
  float vertex_data[array_size][3];
  float normal_data[array_size][3];
  float texture_data[array_size][2];


  float min_height, max_height;
  
};

} /* namespace Sear */
#endif /* SEAR_PATCH_H */
