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
  explicit SPtr(T *t = NULL) :
    m_spd(NULL)
  {
    if (t) m_spd = new SPtrData(t, 1);
  }

  SPtr(const SPtr &sp) throw() {
    aquire(sp.m_spd);
  }

  ~SPtr() throw() { release(); }

  SPtr &operator=(const SPtr &sp) {
    if (this != &sp) {
      release();
      aquire(sp.m_spd);
    }
    return *this;
  }

  const T &operator*() const {
    return *m_spd->ptr;
  }

  T &operator*() {
    return *m_spd->ptr;
  }

  const T *operator->() const {
    return m_spd->ptr;
  }

  T *operator->() {
    return m_spd->ptr;
  }

  operator bool() const {
    return m_spd && m_spd->ptr != NULL;
  }

  bool operator!() const {
    return !m_spd || m_spd->ptr == NULL;
  }

  const T *get() const { return m_spd->ptr; }

  bool isValid() const {
    return ((m_spd) && (m_spd->ptr != NULL));
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

  void release() throw() {
    if (m_spd) {
      --m_spd->cnt;
      if (m_spd->cnt == 0) {
        delete m_spd->ptr;
        delete m_spd;
      }
      m_spd = NULL;
    }
  }

  SPtrData *m_spd;

};


// Same as above, but calls shutdown if required.
// Theres probably a nicer way of combining the two.

template<class T>
class SPtrShutdown {
public:
  explicit SPtrShutdown(T *t = NULL) :
    m_spd(NULL)
  {
    if (t) m_spd = new SPtrData(t, 1);
  }

  SPtrShutdown(const SPtrShutdown &sp) throw() {
    aquire(sp.m_spd);
  }

  ~SPtrShutdown() throw() { release(); }

  SPtrShutdown &operator=(const SPtrShutdown &sp) {
    if (this != &sp) {
      release();
      aquire(sp.m_spd);
    }
    return *this;
  }

  const T &operator*() const {
    return *m_spd->ptr;
  }

  T &operator*() {
    return *m_spd->ptr;
  }

  const T *operator->() const {
    return m_spd->ptr;
  }

  T *operator->() {
    return m_spd->ptr;
  }

  operator bool() const {
    return m_spd && m_spd->ptr != NULL;
  }

  bool operator!() const {
    return !m_spd || m_spd->ptr == NULL;
  }

  const T *get() const { return m_spd->ptr; }

  bool isValid() const {
    return ((m_spd) && (m_spd->ptr != NULL));
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

  void release() throw() {
    if (m_spd) {
      --m_spd->cnt;
      if (m_spd->cnt == 0) {
        if (m_spd->ptr->isInitialised()) {
          m_spd->ptr->shutdown();
        }
        delete m_spd->ptr;
        delete m_spd;
      }
      m_spd = NULL;
    }
  }

  SPtrData *m_spd;

};

} // namespace Sear

#endif // SEAR_SPTR_H
