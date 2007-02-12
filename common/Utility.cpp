// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: Utility.cpp,v 1.13 2007-02-12 18:21:53 simon Exp $

#include "Utility.h"

#include <wfmath/quaternion.h>
#include <stdio.h>
#include <string>
#include <map>
#include <math.h>


#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

#include "common/types.h"

namespace Sear {

void ReduceToUnit(float vector[3]) {
  float length;
  // Calculate the length of the vector		
  length = sqrt((vector[0]*vector[0]) + (vector[1]*vector[1]) + (vector[2]*vector[2]));
  // Keep the program from blowing up by providing an exceptable
  // value for vectors that may calculated too close to zero.
  if(length == 0.0f) length = 1.0f;
  // Dividing each element by the length will result in a
  // unit normal vector.
  vector[0] /= length;
  vector[1] /= length;
  vector[2] /= length;
}

// ---------------------------------------------------------------------
// Points p1, p2, & p3 specified in counter clock-wise order
//
void calcNormal(float v[3][3], float out[3]) {
  float v1[3],v2[3];
  static const int x = 0;
  static const int y = 1;
  static const int z = 2;

  // Calculate two vectors from the three points
  v1[x] = v[0][x] - v[1][x];
  v1[y] = v[0][y] - v[1][y];
  v1[z] = v[0][z] - v[1][z];

  v2[x] = v[1][x] - v[2][x];
  v2[y] = v[1][y] - v[2][y];
  v2[z] = v[1][z] - v[2][z];

  // Take the cross product of the two vectors to get
  // the normal vector which will be stored in out
  out[x] = v1[y]*v2[z] - v1[z]*v2[y];
  out[y] = v1[z]*v2[x] - v1[x]*v2[z];
  out[z] = v1[x]*v2[y] - v1[y]*v2[x];

  // Normalize the vector (shorten length to one)
  ReduceToUnit(out);
}

void QuatToMatrix(const WFMath::Quaternion & quat, float m[4][4]) {
  float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

  const WFMath::Vector<3> &vec = quat.vector();
  float vec_x = vec.x();
  float vec_y = vec.y();
  float vec_z = vec.z();
  float scalar = quat.scalar();

  // calculate coefficients
  x2 = vec_x + vec_x;
  y2 = vec_y + vec_y;
  z2 = vec_z + vec_z;
  xx = vec_x * x2;
  xy = vec_x * y2;
  xz = vec_x * z2;
  yy = vec_y * y2;
  yz = vec_y * z2;
  zz = vec_z * z2;
  wx = scalar * x2;
  wy = scalar * y2;
  wz = scalar * z2;

  m[0][0] = 1.0f - (yy + zz);
  m[0][1] = xy - wz;
  m[0][2] = xz + wy;
  m[0][3] = 0.0f;
             
  m[1][0] = xy + wz;
  m[1][1] = 1.0f - (xx + zz);
  m[1][2] = yz - wx;
  m[1][3] = 0.0f;
  
  m[2][0] = xz - wy;
  m[2][1] = yz + wx;  
  m[2][2] = 1.0f - (xx + yy);
  m[2][3] = 0.0f;
    
  m[3][0] = 0.0f;
  m[3][1] = 0.0f;
  m[3][2] = 0.0f;
  m[3][3] = 1.0f;  
}

WFMath::AxisBox<3> bboxCheck(WFMath::AxisBox<3> bbox) {
  int count = 0;
  if (bbox.lowCorner().x() + bbox.lowCorner().y() + bbox.lowCorner().z() + bbox.highCorner().x() + bbox.highCorner().y() + bbox.highCorner().z()  == 0.0f) {
    // BBOX has no size!! or is equidistant around origin!!!!!
    WFMath::Point<3> lc(0.0f, 0.0f, 0.0f);
    WFMath::Point<3> hc(1.0f, 1.0f, 1.0f);
    bbox = WFMath::AxisBox<3>(lc, hc);
 }
 if (bbox.highCorner().x() > bbox.lowCorner().x()) count++;
 if (bbox.highCorner().y() < bbox.lowCorner().y()) count++;
 if (bbox.highCorner().z() < bbox.lowCorner().z()) count++;
 
 if (count == 0 || count == 2) return bbox;
 else return WFMath::AxisBox<3>(bbox.highCorner(), bbox.lowCorner());	  
}

void Tokeniser::initTokens(const std::string &String) {
  m_String = String;
  m_Begin = 0;
  m_End = 0;
}

std::string Tokeniser::nextToken() {
  std::string sReturn = "";
  bool bInString = false;
  bool bIsEscaped = false;
  
  for(m_Begin = m_End; (m_Begin < m_String.length()) && (m_String[m_Begin] == ' '); ++m_Begin);
  if(m_Begin == m_String.length()) {
    m_End = m_Begin;
    
    return "";
  }
  for(m_End = m_Begin; m_End < m_String.length(); ++m_End) {
    if((m_String[m_End] == '\\') && (bIsEscaped == false)) {
      bIsEscaped = true;
    } else {
      if((m_String[m_End] == ' ') && (bInString == false) && (bIsEscaped == false)) {
        break;
      } else if((m_String[m_End] == '"') && (bIsEscaped == false)) {
        bInString = (bInString == false);
      } else {
        sReturn += m_String[m_End];
      }
      bIsEscaped = false;
    }
  }
  
  return sReturn;
}

std::string Tokeniser::remainingTokens() {
  for(m_Begin = m_End; (m_Begin < m_String.length()) && (m_String[m_Begin] == ' '); ++m_Begin);
    
  return m_String.substr(m_Begin);
}

unsigned char *xpm_to_image(const char *image[], unsigned int &width, unsigned int &height) {
  unsigned int i, row, col;
  unsigned int num_colours = 0;
  unsigned int unknown = 0;
  unsigned int index = 0;
  sscanf(&image[0][0], "%u %u %u %u", &width, &height, &num_colours, &unknown);
  std::map<char, unsigned int> colour_map;
  for (index = 1; index <= num_colours; index++) {
    char code = image[index][0];
    std::string colour_name = std::string(image[index]).substr(4);
    unsigned int colour = 0;
    if (colour_name == "None") colour = 0x00000000;
    else {
      sscanf(colour_name.c_str(), "#%x", &colour);
      colour <<= 8;
      colour |= 0xFF;
    }
    colour_map[code] = colour;
  }
//  unsigned char *data = (unsigned char *)malloc(width * height * 4 * sizeof(char));
  unsigned char *data = new unsigned char [width * height * 4];
  i = 0;
  for ( row=0; row < height; ++row ) {
    for ( col=0; col < width; ++col ) {
      unsigned int colour = colour_map[image[height - row - 1 + index][col]];
      data[i++] = (colour & 0xFF000000) >> 24;
      data[i++] = (colour & 0x00FF0000) >> 16;
      data[i++] = (colour & 0x0000FF00) >> 8;
      data[i++] = (colour & 0x000000FF);
    }	
  } 
  return data;  
}

OrientBBox::OrientBBox()
{
    for (unsigned int P=0; P < LAST_POSITION; ++P) {
      points[P] = WFMath::Vector<3>(0,0,0);
    }
}

OrientBBox::OrientBBox(const WFMath::AxisBox<3>& ab)
{
  const WFMath::Point<3> &high = ab.highCorner();
  const WFMath::Point<3> &low = ab.lowCorner();

  points[UPPER_LEFT_FRONT]  = WFMath::Vector<3>(high.x(), high.y(), high.z());
  points[UPPER_RIGHT_FRONT] = WFMath::Vector<3>(low.x(), high.y(), high.z());
  points[UPPER_LEFT_BACK]   = WFMath::Vector<3>(high.x(), low.y(), high.z());
  points[UPPER_RIGHT_BACK]  = WFMath::Vector<3>(low.x(), low.y(), high.z());

  points[LOWER_LEFT_FRONT]  = WFMath::Vector<3>(high.x(), high.y(), low.z());
  points[LOWER_RIGHT_FRONT] = WFMath::Vector<3>(low.x(), high.y(), low.z());
  points[LOWER_LEFT_BACK]   = WFMath::Vector<3>(high.x(), low.y(), low.z());
  points[LOWER_RIGHT_BACK]  = WFMath::Vector<3>(low.x(), low.y(), low.z());
}

void OrientBBox::rotate(const WFMath::Quaternion& q)
{
  for (unsigned int P=0; P < LAST_POSITION; ++P) {
    points[P] = points[P].rotate(q);
  }
}

} /* namespace Sear */
