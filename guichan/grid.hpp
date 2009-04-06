// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Simon Goodall

#ifndef GCN_GRID_HPP
#define GCN_GRID_HPP

#include "box.hpp"

namespace gcn
{
    /**
     * This is the box container base class. It is a widget that holds other
     * widgets in an ordered row or column. It is abstract and should not be
     * instantiated. The pattern is from gtk+, including some of the comments.
     */
    class GCN_CORE_DECLSPEC Grid: public Box
    {
    public:

         Grid(int w, int h) :
m_width(w), m_height(h) {}
        /**
         * Destructor
         */
        virtual ~Grid();

        virtual void pack(Widget* child, int padding = 0);

      private:
 int m_width;
      int m_height;

    };
} // end gcn

#endif // end GCN_GRID_HPP
