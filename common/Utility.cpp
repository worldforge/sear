// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Utility.cpp,v 1.4 2002-10-21 20:12:04 simon Exp $

#include "Utility.h"

#include <wfmath/quaternion.h>
#include <stdio.h>
#include <string>
#include <map>
#include <math.h>

#ifdef DEBUG
  #include "common/mmgr.h"
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

const std::string Tokeniser::delimeters = " ";

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

void QuatToMatrix(WFMath::Quaternion quat, float m[4][4]) {
  float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
  // calculate coefficients
  x2 = quat.vector().x() + quat.vector().x();
  y2 = quat.vector().y() + quat.vector().y();
  z2 = quat.vector().z() + quat.vector().z();
  xx = quat.vector().x() * x2;
  xy = quat.vector().x() * y2;
  xz = quat.vector().x() * z2;
  yy = quat.vector().y() * y2;
  yz = quat.vector().y() * z2;
  zz = quat.vector().z() * z2;
  wx = quat.scalar() * x2;
  wy = quat.scalar() * y2;
  wz = quat.scalar() * z2;

  m[0][0] = 1.0 - (yy + zz);
  m[0][1] = xy - wz;
  m[0][2] = xz + wy;
  m[0][3] = 0.0;
             
  m[1][0] = xy + wz;
  m[1][1] = 1.0 - (xx + zz);
  m[1][2] = yz - wx;
  m[1][3] = 0.0;
  
  m[2][0] = xz - wy;
  m[2][1] = yz + wx;  
  m[2][2] = 1.0 - (xx + yy);
  m[2][3] = 0.0;
    
  m[3][0] = 0;
  m[3][1] = 0;
  m[3][2] = 0;
  m[3][3] = 1;  
}

WFMath::AxisBox<3> bboxCheck(WFMath::AxisBox<3> bbox) {
  int count = 0;
  if (bbox.lowCorner().x() + bbox.lowCorner().y() + bbox.lowCorner().z() + bbox.highCorner().x() + bbox.highCorner().y() + bbox.highCorner().z()  == 0.0f) {
    // BBOX has no size!! or is equidistant sround origin!!!!!
    WFMath::Point<3> lc = WFMath::Point<3>(0.0f, 0.0f, 0.0f);
    WFMath::Point<3> hc = WFMath::Point<3>(1.0f, 1.0f, 1.0f);
    bbox = WFMath::AxisBox<3>(lc, hc);
 }
 if (bbox.highCorner().x() > bbox.lowCorner().x()) count++;
 if (bbox.highCorner().y() < bbox.lowCorner().y()) count++;
 if (bbox.highCorner().z() < bbox.lowCorner().z()) count++;
 
 if (count == 0 || count == 2) return bbox;
 else return WFMath::AxisBox<3>(bbox.highCorner(), bbox.lowCorner());	  
}

void Tokeniser::initTokens(const std::string &tokens) {
  token_string = tokens;
  last_pos = token_string.find_first_not_of(delimeters, 0);
  pos = token_string.find_first_of(delimeters, last_pos);
}

std::string Tokeniser::nextToken() {
  if (last_pos == std::string::npos) return "";
  std::string token = token_string.substr(last_pos, pos - last_pos);
  last_pos = token_string.find_first_not_of(delimeters, pos);
  pos = token_string.find_first_of(delimeters, last_pos);
  return token;
}

std::string Tokeniser::remainingTokens() {
  if (last_pos == std::string::npos) return "";
  return token_string.substr(last_pos, token_string.size() - last_pos);
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
  unsigned char *data = (unsigned char *)malloc(width * height * 4 * sizeof(char));
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

} /* namespace Sear */
