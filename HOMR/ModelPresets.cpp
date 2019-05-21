//
//  ModelPresets.cpp
//  HOMR2
//
//  Created by Dale Johnson on 29/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#include "ModelPresets.hpp"

Model rectangularHall(float height, float width, float depth,
                      const Vec8f& leftWallAbsorb, const Vec8f& frontWallAbsorb,
                      const Vec8f& rightWallAbsorb, const Vec8f& rearWallAbsorb,
                      const Vec8f& floorAbsorb, const Vec8f& ceilAbsorb)
{
    Model hall;
    Polygon leftWall;
    Polygon frontWall;
    Polygon rightWall;
    Polygon rearWall;
    Polygon floor;
    Polygon ceil;
    
    leftWall.addVertex(Vec3f(0.f, 0.f, 0.f));
    leftWall.addVertex(Vec3f(0.f, height, 0.f));
    leftWall.addVertex(Vec3f(0.f, height, depth));
    leftWall.addVertex(Vec3f(0.f, 0.f, depth));
    leftWall.setAbsorbCoeffs(leftWallAbsorb);
    hall.appendSurfaces(triangulatePolygon(leftWall));
    
    frontWall.addVertex(Vec3f(0.f, 0.f, depth));
    frontWall.addVertex(Vec3f(0.f, height, depth));
    frontWall.addVertex(Vec3f(width, height, depth));
    frontWall.addVertex(Vec3f(width, 0.f, depth));
    frontWall.setAbsorbCoeffs(frontWallAbsorb);
    hall.appendSurfaces(triangulatePolygon(frontWall));
    
    rightWall.addVertex(Vec3f(width, 0.f, 0.f));
    rightWall.addVertex(Vec3f(width, height, 0.f));
    rightWall.addVertex(Vec3f(width, height, depth));
    rightWall.addVertex(Vec3f(width, 0.f, depth));
    rightWall.setAbsorbCoeffs(rightWallAbsorb);
    hall.appendSurfaces(triangulatePolygon(rightWall));
    
    rearWall.addVertex(Vec3f(0.f, 0.f, 0.f));
    rearWall.addVertex(Vec3f(0.f, height, 0.f));
    rearWall.addVertex(Vec3f(width, height, 0.f));
    rearWall.addVertex(Vec3f(width, 0.f, 0.f));
    rearWall.setAbsorbCoeffs(rearWallAbsorb);
    hall.appendSurfaces(triangulatePolygon(rearWall));
    
    floor.addVertex(Vec3f(0.f, 0.f, 0.f));
    floor.addVertex(Vec3f(0.f, 0.f, depth));
    floor.addVertex(Vec3f(width, 0.f, depth));
    floor.addVertex(Vec3f(width, 0.f, 0.f));
    floor.setAbsorbCoeffs(floorAbsorb);
    hall.appendSurfaces(triangulatePolygon(floor));
    
    ceil.addVertex(Vec3f(0.f, height, 0.f));
    ceil.addVertex(Vec3f(0.f, height, depth));
    ceil.addVertex(Vec3f(width, height, depth));
    ceil.addVertex(Vec3f(width, height, 0.f));
    ceil.setAbsorbCoeffs(ceilAbsorb);
    hall.appendSurfaces(triangulatePolygon(ceil));
    
    return hall;
}