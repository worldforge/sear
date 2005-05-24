// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

#include "AreaModel.h"
#include "ObjectRecord.h"
#include "environment/Environment.h"
#include <Mercator/Area.h>
#include "src/WorldEntity.h"
#include <iostream>

namespace Sear
{

typedef WFMath::Point<2> Point2;
typedef WFMath::Vector<3> Vector3;

AreaModel::AreaModel(Render* r, ObjectRecord* orec) :
    Model(r),
    m_object(orec),
    m_area(NULL)
{
}

AreaModel::~AreaModel()
{

}

bool AreaModel::init()
{
    WorldEntity* we = m_object->entity;
    
    if (!we->hasAttr("area")) {
        std::cerr << "AreaModel defined on entity with no area attribute" << std::endl;
        return false;
    }
    
    const Atlas::Message::MapType& areaData(we->valueOfAttr("area").asMap());
    Atlas::Message::MapType::const_iterator it = areaData.find("points");
    if ((it == areaData.end()) || !it->second.isList()) {
        std::cerr << "malformed area attribute on entity, no points data" << std::endl;
        return false;
    }
    
    const Atlas::Message::ListType& pointsData(it->second.asList());
    it = areaData.find("layer");
    if ((it == areaData.end()) || !it->second.isInt()) {
        std::cerr << "malformed area attribute on entity, no layer data" << std::endl;
        return false;
    }

    int layer = it->second.asInt();
    m_area = new Mercator::Area(layer, false);
       
    WFMath::Polygon<2> poly;
    for (unsigned int p=0; p<pointsData.size(); ++p) {
        if (!pointsData[p].isList()) {
            std::cerr << "skipped malformed point in area" << std::endl;
            continue;
        }
        
        const Atlas::Message::ListType& point(pointsData[p].asList());
        if ((point.size() < 2) || !point[0].isFloat() || !point[1].isFloat()) {
            std::cerr << "skipped malformed point in area" << std::endl;
            continue;
        }
        
        Point2 wpt(point[0].asFloat(), point[1].asFloat());
        poly.addCorner(poly.numCorners(), wpt);
    }
    
// transform polygon into terrain coords
    Vector3 xVec = Vector3(1.0, 0.0, 0.0).rotate(m_object->orient);
    double theta = atan2(xVec.y(), xVec.x()); // rotation about Z
    
    WFMath::RotMatrix<2> rm;
    poly.rotatePoint(rm.rotation(theta), Point2(0,0));
    poly.shift(WFMath::Vector<2>(m_object->position.x(), m_object->position.y()));
    
    m_area->setShape(poly);
    Environment::getInstance().registerArea(m_area);
    return true;
}

void AreaModel::invalidate()
{
}

int AreaModel::shutdown()
{
    return 0;
}

int AreaModel::getLayer() const
{
    return m_area->getLayer();
}

} // of namespace Sear
