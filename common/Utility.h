// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Utility.h,v 1.11 2004-04-26 16:01:35 simon Exp $

#ifndef SEAR_UTILITY_H
#define SEAR_UTILITY_H 1

#include <wfmath/const.h>
#include <wfmath/quaternion.h>
#include <wfmath/axisbox.h>
#include <string>

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <deque>

#ifdef HAVE_SSTREAM
#include <sstream>
#elif defined(HAVE_STRSTREAM)
#include <strstream>
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

#ifdef HAVE_SSTREAM
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
  std::stringstream ss;
  ss << t;
  return ss.str();
}
#endif

#ifdef HAVE_STRSTREAM
template <class out_value, class in_value>
void cast_stream(const in_value &in, out_value &out) {
  std::strstream ss;
  ss << in << std::ends;
  ss >> out;
}

template <class T>
std::string string_fmt(const T & t) {
  std::strstream ss;
  ss << t << std::ends;
  return ss.str();
}
#endif
/*
double SineSeries (double fRad, int i, int iMaxTerms) {
  if (i > iMaxTerms) return 1.0;

  return 1.0 - (fRad * fRad / (2.0 * i + 2.0) / (2.0 * i + 3.0) * SineSeries (fRad, i + 1, iMaxTerms));
}

double Sine(double fRad) {
  const int iMaxTerms = 10;
  return fRad * SineSeries(fRad, 0, iMaxTerms);
}

*/

/*
template <R, i> struct Sine {
  enum { Val = 1.0 - (R * R / (2.0 * i + 2.0) / (2.0 * i + 3.0) * Sine<R, i + i >) };
};

template <> struct Sine<R, iMaxTerms > {
  enum { Val = 1.0; };
};
#define SineT(n) Sine<n>::Val

*/

template<int N, int I>
class Sine {
public:
    static inline float sin()
    {
        return (I*2*M_PI/N) * SineSeries<N,I,10,0>::accumulate();
    }
};


// Compute J terms in the series expansion.  K is the loop variable.
template<int N, int I, int J, int K>
class SineSeries {
public:
    enum { go = (K+1 != J) };

    static inline float accumulate()
    {
        return 1-(I*2*M_PI/N)*(I*2*M_PI/N)/(2*K+2)/(2*K+3) *
            SineSeries<N*go,I*go,J*go,(K+1)*go>::accumulate();
    }
};


// Specialization to terminate loop
class SineSeries<0,0,0,0> {
public:
    static inline float accumulate()
    { return 1; }
};

#define SineT(n) Sine<n, 10>::sin()


template <class T>
const T SQR(const T &t) { return t * t; }

void reduceToUnit(float vector[3]);
void calcNormal(float v[3][3], float out[3]);

void QuatToMatrix(WFMath::Quaternion quat, float m[4][4]);

WFMath::AxisBox<3> bboxCheck(WFMath::AxisBox<3> bbox);

unsigned char *xpm_to_image(const char * image[], unsigned int &width, unsigned int &height);

class Tokeniser {
public:
  Tokeniser() {}
  ~Tokeniser() {}

  void initTokens(const std::string &tokens);
  std::string nextToken();
  std::string remainingTokens();
  
protected:
  std::string::size_type pos, last_pos;
  std::string token_string;
  static const std::string delimeters;
  static const std::string quotes;
  bool quoted;
};
  
} /* namespace Sear */
#endif /* SEAR_UTILITY_H */

