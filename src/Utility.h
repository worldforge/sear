// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _UTILITY_H_
#define _UTILITY_H_ 1

#include <wfmath/const.h>
#include <wfmath/quaternion.h>
#include <wfmath/axisbox.h>
//class Quaternion;
#include <string>
#include "config.h"

#include <deque>

#ifdef HAVE_SSTREAM
#include <sstream>
#define END_STREAM "" 
typedef std::stringstream SSTREAM;
#elif defined(HAVE_STRSTREAM)
#include <strstream>
typedef std::strstream SSTREAM;
// strstream requires a null character at the end of the stream
#define END_STREAM std::ends
#else
#error "sstream or strstream not found!"
#endif

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
  SSTREAM ss;
  ss << in << END_STREAM;
#ifdef HAVE_SSTREAM
  SSTREAM sss(ss.str());
  sss >> out;
#else
  ss >> out;
#endif
}

#define SQR(X) ((X) * (X))

template <class T>
std::string string_fmt(const T & t) {
  SSTREAM ss;
  ss << t << END_STREAM;
  return ss.str();
}

//std::string floatToString(float &);
//std::string intToString(int &);

void reduceToUnit(float vector[3]);

void calcNormal(float v[3][3], float out[3]);

WFMath::Quaternion MatToQuat(float m[4][4]);

//void QuatToMatrix(WFMath::Quaternion quat, float m[4][4]);
void QuatToMatrix(WFMath::Quaternion quat, float m[4][4]);

WFMath::Quaternion EulerToQuat(float roll, float pitch, float yaw);

WFMath::Quaternion QuatMul(const WFMath::Quaternion &q1, const WFMath::Quaternion &q2);

WFMath::Quaternion QuatSlerp(WFMath::Quaternion from, WFMath::Quaternion to, float t);

WFMath::AxisBox<3> bboxCheck(WFMath::AxisBox<3> bbox);

void tokenise(std::deque<std::string> &tokens, const std::string &input);

unsigned char *xpm_to_image(const char * image[], unsigned int &width, unsigned int &height);

class Tokeniser {
public:
  Tokeniser() {}
  ~Tokeniser() {}

  void initTokens(const std::string &tokens);
  std::string Tokeniser::nextToken();
  std::string Tokeniser::remainingTokens();
 
  
protected:
  std::string::size_type pos, last_pos;
  std::string token_string;
  static const std::string delimeters;
  static const std::string quotes;
  bool quoted;
};

  
} /* namespace Sear */
#endif /* _UTILITY_H_ */

