//
//  occlusionTest.cpp
//  HOMR2
//
//  Created by Dale Johnson on 15/07/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#include "FastISM.hpp"
#include <vector>
using namespace std;

int main()
{
    Model model;
    Polygon poly;
    vector<Triangle> surfaces;
    
    poly.addVertex(Vec3f(0.f, 0.f, 0.f));
    poly.addVertex(Vec3f(0.f, 2.f, 0.f));
    poly.addVertex(Vec3f(0.f, 2.f, 2.f));
    poly.addVertex(Vec3f(0.f, 0.f, 2.f));
    surfaces = triangulatePolygon(poly);
    model.appendSurfaces(surfaces);
    poly._vertices.clear();
    
    poly.addVertex(Vec3f(0.f, 0.f, 2.f));
    poly.addVertex(Vec3f(0.f, 2.f, 2.f));
    poly.addVertex(Vec3f(2.f, 2.f, 2.f));
    poly.addVertex(Vec3f(2.f, 0.f, 2.f));
    surfaces = triangulatePolygon(poly);
    model.appendSurfaces(surfaces);
    poly._vertices.clear();
    
    poly.addVertex(Vec3f(2.f, 0.f, 0.f));
    poly.addVertex(Vec3f(2.f, 2.f, 0.f));
    poly.addVertex(Vec3f(2.f, 2.f, 2.f));
    poly.addVertex(Vec3f(2.f, 0.f, 2.f));
    surfaces = triangulatePolygon(poly);
    model.appendSurfaces(surfaces);
    poly._vertices.clear();
    
    poly.addVertex(Vec3f(0.f, 0.f, 0.f));
    poly.addVertex(Vec3f(0.f, 2.f, 0.f));
    poly.addVertex(Vec3f(2.f, 2.f, 0.f));
    poly.addVertex(Vec3f(2.f, 0.f, 0.f));
    surfaces = triangulatePolygon(poly);
    model.appendSurfaces(surfaces);
    poly._vertices.clear();
    
    poly.addVertex(Vec3f(0.f, 0.f, 0.f));
    poly.addVertex(Vec3f(0.f, 0.f, 2.f));
    poly.addVertex(Vec3f(2.f, 0.f, 2.f));
    poly.addVertex(Vec3f(2.f, 0.f, 0.f));
    surfaces = triangulatePolygon(poly);
    model.appendSurfaces(surfaces);
    poly._vertices.clear();
    
    poly.addVertex(Vec3f(0.f, 2.f, 0.f));
    poly.addVertex(Vec3f(0.f, 2.f, 2.f));
    poly.addVertex(Vec3f(2.f, 2.f, 2.f));
    poly.addVertex(Vec3f(2.f, 2.f, 0.f));
    surfaces = triangulatePolygon(poly);
    model.appendSurfaces(surfaces);
    poly._vertices.clear();
    
    Vec3f recOrigin(1.0f, 1.0f, 0.5f);
    Vec3f srcOrigin(1.0f, 1.0f, 1.f);
    Receiver rec(recOrigin, Vec3f(0.0, 0.0, 1.0), 0.5, POLAR_PATTERN_CARDIOID);
    Source src(srcOrigin, Vec3f(0.0, 0.0, 1.0), Vec8f(1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f));
    model.addReceiver(rec);
    model._source = src;
    
    RawResponse ir(1);
    
    ism(model, ir, 1);
    
    return 0;
}
