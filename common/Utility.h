// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: Utility.h,v 1.18 2006-04-30 15:47:15 simon Exp $

#ifndef SEAR_UTILITY_H
#define SEAR_UTILITY_H 1

#include <wfmath/const.h>
#include <wfmath/quaternion.h>
#include <wfmath/axisbox.h>

#include <string>
#include <deque>
#include <sstream>

namespace Sear {

template <class T>
T deg_to_rad(const T & t) {
  return (t * (WFMath::Pi / 180.0f));
}
	
template <class T>
T rad_to_deg(const T & t) {
  return (t * (180.0f / WFMath::Pi));
}

template <class T>
T square(T t) {
  return (t * t);
}

template <class out_value, class in_value>
void cast_stream(const in_value &in, out_value &out) {
  std::stringstream ss;
  ss << in;
#ifdef NEEDS_SSTREAM_WORKAROUND
  std::stringstream sss(ss.str());
  sss >> out;
#else
  ss >> out;
#endif
}

template <class T>
std::string string_fmt(const T & t) {
  std::ostringstream ss;
  ss << t;
  return ss.str();
}


template <class T>
const T SQR(const T &t) { return t * t; }

void reduceToUnit(float vector[3]);
void calcNormal(float v[3][3], float out[3]);

void QuatToMatrix(const WFMath::Quaternion & quat, float m[4][4]);

WFMath::AxisBox<3> bboxCheck(WFMath::AxisBox<3> bbox);

unsigned char *xpm_to_image(const char * image[], unsigned int &width, unsigned int &height);

class Tokeniser {
public:
  Tokeniser(const std::string &String = "") : m_String(String), m_Begin(0), m_End(0) {
  }
  
  ~Tokeniser() {
  }
  
  void initTokens(const std::string &String);
  std::string nextToken();
  std::string remainingTokens();
protected:
  std::string m_String;
  std::string::size_type m_Begin;
  std::string::size_type m_End;
};
  
} /* namespace Sear */
#endif /* SEAR_UTILITY_H */

