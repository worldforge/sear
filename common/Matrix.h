// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: Matrix.h,v 1.3 2006-10-25 17:28:39 simon Exp $

#ifndef SEAR_MATRIX_H
#define SEAR_MATRIX_H 1

namespace Sear {

class Matrix {
public:
  void setMatrix(float m[4][4]) {
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        m_matrix[i][j] = m[i][j];
      }
    }
  }

  void getMatrix(float m[4][4]) const {
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        m[i][j] = m_matrix[i][j];
      }
    }
  }

  void identity() {
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        if (i == j)  m_matrix[i][j] = 1.0f;
        else  m_matrix[i][j] = 0.0f;
      }
    }
  }

  void scalev(float s[4]) {
    for (int i = 0; i < 4; ++i) {
      m_matrix[i][i] *= s[i];
    }
  }

  void scale(float s) {
    for (int i = 0; i < 4; ++i) {
      m_matrix[i][i] *= s;
    }
  }

  void translate(float x, float y, float z) {
    m_matrix[3][0] += x;
    m_matrix[3][1] += y;
    m_matrix[3][2] += z;
//    m_matrix[0][3] += x;
//    m_matrix[1][3] += y;
//    m_matrix[2][3] += z;
  }

  void rotateX(float angle) {
    float m[4][4];
    m[0][0] = 1.0f; m[0][1] = 0.0f;       m[0][2] = 0.0f;        m[0][3] = 0.0f;
    m[1][0] = 0.0f; m[1][1] = cos(angle); m[1][2] = -sin(angle); m[1][3] = 0.0f;
    m[2][0] = 0.0f; m[2][1] = sin(angle); m[2][2] =  cos(angle); m[2][3] = 0.0f;
    m[3][0] = 0.0f; m[3][1] = 0.0f;       m[3][2] = 0.0f;        m[3][3] = 1.0f;

    multMatrix(m);
  }

  void rotateY(float angle) {
    float m[4][4];
    m[0][0] =  cos(angle); m[0][1] = 0.0f; m[0][2] = sin(angle); m[0][3] = 0.0f;
    m[1][0] =  sin(angle); m[1][1] = 0.0f; m[1][2] = 0.0f;       m[1][3] = 0.0f;
    m[2][0] = -sin(angle); m[2][1] = 0.0f; m[2][2] = cos(angle); m[2][3] = 0.0f;
    m[3][0] =  0.0f;       m[3][1] = 0.0f; m[3][2] = 0.0f;       m[3][3] = 1.0f;

    multMatrix(m);
  }

  void rotateZ(float angle) {
    float m[4][4];
    m[0][0] = cos(angle); m[0][1] = -sin(angle); m[0][2] = 0.0f; m[0][3] = 0.0f;
    m[1][0] = sin(angle); m[1][1] =  cos(angle); m[1][2] = 0.0f; m[1][3] = 0.0f;
    m[2][0] = 0.0f;       m[2][1] = 0.0f;        m[2][2] = 1.0f; m[2][3] = 0.0f;
    m[3][0] = 0.0f;       m[3][1] = 0.0f;        m[3][2] = 0.0f; m[3][3] = 1.0f;

    multMatrix(m);
  }

  void multMatrix(float n[4][4]) {
    float m[4][4];
    // Copy orignal matrix so we can put new values directly into member var.
    getMatrix(m);
    m_matrix[0][0] = m[0][0] * n[0][0] + m[0][1] * n[1][0] + m[0][2] * n[2][0] + m[0][3] * n[3][0];
    m_matrix[0][1] = m[0][0] * n[0][1] + m[0][1] * n[1][1] + m[0][2] * n[2][1] + m[0][3] * n[3][1];
    m_matrix[0][2] = m[0][0] * n[0][2] + m[0][1] * n[1][2] + m[0][2] * n[2][2] + m[0][3] * n[3][2];
    m_matrix[0][3] = m[0][0] * n[0][3] + m[0][1] * n[1][3] + m[0][2] * n[2][3] + m[0][3] * n[3][3];

    m_matrix[1][0] = m[1][0] * n[0][0] + m[1][1] * n[1][0] + m[1][2] * n[2][0] + m[1][3] * n[3][0];
    m_matrix[1][1] = m[1][0] * n[0][1] + m[1][1] * n[1][1] + m[1][2] * n[2][1] + m[1][3] * n[3][1];
    m_matrix[1][2] = m[1][0] * n[0][2] + m[1][1] * n[1][2] + m[1][2] * n[2][2] + m[1][3] * n[3][2];
    m_matrix[1][3] = m[1][0] * n[0][3] + m[1][1] * n[1][3] + m[1][2] * n[2][3] + m[1][3] * n[3][3];

    m_matrix[2][0] = m[2][0] * n[0][0] + m[2][1] * n[1][0] + m[2][2] * n[2][0] + m[2][3] * n[3][0];
    m_matrix[2][1] = m[2][0] * n[0][1] + m[2][1] * n[1][1] + m[2][2] * n[2][1] + m[2][3] * n[3][1];
    m_matrix[2][2] = m[2][0] * n[0][2] + m[2][1] * n[1][2] + m[2][2] * n[2][2] + m[2][3] * n[3][2];
    m_matrix[2][3] = m[2][0] * n[0][3] + m[2][1] * n[1][3] + m[2][2] * n[2][3] + m[2][3] * n[3][3];

    m_matrix[3][0] = m[3][0] * n[0][0] + m[3][1] * n[1][0] + m[3][2] * n[2][0] + m[3][3] * n[3][0];
    m_matrix[3][1] = m[3][0] * n[0][1] + m[3][1] * n[1][1] + m[3][2] * n[2][1] + m[3][3] * n[3][1];
    m_matrix[3][2] = m[3][0] * n[0][2] + m[3][1] * n[1][2] + m[3][2] * n[2][2] + m[3][3] * n[3][2];
    m_matrix[3][3] = m[3][0] * n[0][3] + m[3][1] * n[1][3] + m[3][2] * n[2][3] + m[3][3] * n[3][3];
  }

protected:
  float m_matrix[4][4];

};


} // namespace Sear

#endif // SEAR_MATRIX_H 
