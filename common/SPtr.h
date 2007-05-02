// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 Simon Goodall

// Based on code at: http://ootips.org/yonat/4dev/smart-pointers.html

#ifndef SEAR_SPTR_H
#define SEAR_SPTR_H 1

#include <cassert>

namespace Sear {

// Standard Ref counting Smart Pointer

template<class T>
class SPtr {
public:
  explicit SPtr(T *t = 0) :
    m_spd(0)
  {
    if (t) m_spd = new SPtrData(t, 1);
  }

  SPtr(const SPtr &sp) throw() {
    aquire(sp.m_spd);
  }

  ~SPtr() throw() { release(); }

  SPtr &operator=(const SPtr &sp) throw() {
    if (this != &sp) {
      release();
      aquire(sp.m_spd);
    }
    return *this;
  }

  const T &operator*() const throw() {
    return *m_spd->ptr;
  }

  T &operator*() throw() {
    return *m_spd->ptr;
  }

  const T *operator->() const throw() {
    return m_spd->ptr;
  }

  T *operator->() throw() {
    return m_spd->ptr;
  }

  operator bool() const throw() {
    return m_spd && m_spd->ptr != 0;
  }

  bool operator!() const throw() {
    return !m_spd || m_spd->ptr == 0;
  }

  const T *get() const throw() { return (m_spd) ? (m_spd->ptr) : (0); }
  T *get() throw() { return (m_spd) ? (m_spd->ptr) : (0); }

  bool isValid() const throw() {
    return ((m_spd) && (m_spd->ptr != 0));
  }

  void release() throw() {
    if (m_spd) {
      --m_spd->cnt;
      if (m_spd->cnt == 0) {
        delete m_spd->ptr;
        delete m_spd;
      }
      m_spd = 0;
    }
  }


private:
  typedef struct t_SPtrData{
    t_SPtrData(T *p, int n) : ptr(p), cnt(n) {}

    T *ptr;
    int cnt;
  } SPtrData;

  void aquire(SPtrData *spd) throw() {
    m_spd = spd;
    if (m_spd) ++m_spd->cnt;
  }

  SPtrData *m_spd;

};

} // namespace Sear

#endif // SEAR_SPTR_H
