// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "Utility.h"

#include <wfmath/quaternion.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#define DELTA 0.0
#include <math.h>

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

WFMath::Quaternion MatToQuat(float m[4][4]) {
  float  tr, s, q[4];
  int    i, j, k;
  int nxt[3] = {1, 2, 0};
  float x, y, z, w;
  
  tr = m[0][0] + m[1][1] + m[2][2];

  // check the diagonal
  if (tr > 0.0) {
    s = sqrt (tr + 1.0);
    w = s / 2.0;
    s = 0.5 / s;
    x = (m[1][2] - m[2][1]) * s;
    y = (m[2][0] - m[0][2]) * s;
    z = (m[0][1] - m[1][0]) * s;
  } else {		
    // diagonal is negative
    i = 0;
    if (m[1][1] > m[0][0]) i = 1;
    if (m[2][2] > m[i][i]) i = 2;		
    j = nxt[i];
    k = nxt[j];
    s = sqrt ((m[i][i] - (m[j][j] + m[k][k])) + 1.0);    
    q[i] = s * 0.5;
    if (s != 0.0) s = 0.5 / s;
    q[3] = (m[j][k] - m[k][j]) * s;
    q[j] = (m[i][j] + m[j][i]) * s;
    q[k] = (m[i][k] + m[k][i]) * s;
    x = q[0];
    y = q[1];
    z = q[2];
    w = q[3];
  }
  return WFMath::Quaternion(w, x, y, z);
}

//void QuatToMatrix(WFMath::Quaternion quat, float m[4][4]) {
void QuatToMatrix(WFMath::Quaternion quat, float m[4][4]) {
//  float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
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


WFMath::Quaternion EulerToQuat(float roll, float pitch, float yaw) {
  float cr, cp, cy, sr, sp, sy, cpcy, spsy;
  float w, x, y, z;
  // calculate trig identities
  cr = cos(roll/2);
  cp = cos(pitch/2);
  cy = cos(yaw/2);
  sr = sin(roll/2);
  sp = sin(pitch/2);
  sy = sin(yaw/2);
		
  cpcy = cp * cy;
  spsy = sp * sy;
		
  w = cr * cpcy + sr * spsy;
  x = sr * cpcy - cr * spsy;
  y = cr * sp * cy + sr * cp * sy;
  z = cr * cp * sy - sr * sp * cy;

  return WFMath::Quaternion(w, x , y, z);
}


WFMath::Quaternion QuatMul(const WFMath::Quaternion &q1, const WFMath::Quaternion &q2) {
  float A, B, C, D, E, F, G, H;
  float w, x, y, z;
  A = (q1.scalar() + q1.vector().x())*(q2.scalar() + q2.vector().x());
  B = (q1.vector().z() - q1.vector().y())*(q2.vector().y() - q2.vector().z());
  C = (q1.scalar() - q1.vector().x())*(q2.vector().y() + q2.vector().z()); 
  D = (q1.vector().y() + q1.vector().z())*(q2.scalar() - q2.vector().x());
  E = (q1.vector().x() + q1.vector().z())*(q2.vector().x() + q2.vector().y());
  F = (q1.vector().x() - q1.vector().z())*(q2.vector().x() - q2.vector().y());
  G = (q1.scalar() + q1.vector().y())*(q2.scalar() - q2.vector().z());
  H = (q1.scalar() - q1.vector().y())*(q2.scalar() + q2.vector().z());

  w = B + (-E - F + G + H) /2;
  x = A - (E + F + G + H)/2; 
  y = C + (E - F + G - H)/2; 
  z = D + (E - F - G + H)/2;

  return WFMath::Quaternion(w, x, y, z);
}

WFMath::Quaternion QuatSlerp(WFMath::Quaternion from, WFMath::Quaternion to, float t) {
  
  float to1[4];
  double omega, cosom, sinom, scale0, scale1;
  float w, x, y, z;

  // calc cosine
  cosom = from.vector().x() * to.vector().x() + from.vector().y() * to.vector().y() + from.vector().z() * to.vector().z() + from.scalar() * to.scalar();
  				      
  // adjust signs (if necessary)
				      
  if ( cosom <0.0 ){ cosom = -cosom; to1[0] = - to.vector().x();
    to1[1] = - to.vector().y();
    to1[2] = - to.vector().z();    
    to1[3] = - to.scalar();    
  } else  {
     to1[0] = to.vector().x();     
     to1[1] = to.vector().y();     
     to1[2] = to.vector().z();   
     to1[3] = to.scalar();	
  }
  
  
  // calculate coefficients
  if ( (1.0 - cosom) > DELTA ) {				      
    // standard case (slerp)
    omega = acos(cosom);
    sinom = sin(omega);
    scale0 = sin((1.0 - t) * omega) / sinom;
    scale1 = sin(t * omega) / sinom;
  } else {        
    // "from" and "to" quaternions are very close 
    //  ... so we can do a linear interpolation
    scale0 = 1.0 - t;
    scale1 = t;
  }  
  // calculate final values
  x = scale0 * from.vector().x() + scale1 * to1[0];
  y = scale0 * from.vector().y() + scale1 * to1[1];
  z = scale0 * from.vector().z() + scale1 * to1[2];
  w = scale0 * from.scalar() + scale1 * to1[3]; 

  return WFMath::Quaternion(w, x, y, z);
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
//  try {
    last_pos = token_string.find_first_not_of(delimeters, 0);
    pos = token_string.find_first_of(delimeters, last_pos);
//  } catch (...) {
//
//  }
 
}

std::string Tokeniser::nextToken() {
  if (last_pos == std::string::npos) return "";
//  try {
    std::string token = token_string.substr(last_pos, pos - last_pos);
    last_pos = token_string.find_first_not_of(delimeters, pos);
    pos = token_string.find_first_of(delimeters, last_pos);
    return token;
//  } catch (...) {
//    return "";
//  }
}

std::string Tokeniser::remainingTokens() {
  if (last_pos == std::string::npos) return "";
//try {  
    return token_string.substr(last_pos, token_string.size() - last_pos);
//  } catch (...) {
//    return "";
//  }
}                                
  
unsigned char *xpm_to_image(const char *image[], unsigned int &width, unsigned int &height) {
  unsigned int i, row, col;
  unsigned int num_colours = 0;
  unsigned int unknown = 0;
  unsigned int index = 0;
  std::cout << image[0] << std::endl;
//  sscanf(image[0], "%u", &width);//, &height, &num_colours, &unknown);
  sscanf(&image[0][0], "%u %u %u %u", &width, &height, &num_colours, &unknown);
  std::cout << "Width: " << width << " Height: " << height << " Num Colours: " << num_colours << std::endl;
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
    std::cout << "Colour " << code << " is " << colour_name << ", " << colour << std::endl;
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
