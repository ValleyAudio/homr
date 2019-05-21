//
//  GeometricFunctions.hpp
//
//  Created by Dale Johnson on 14/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#ifndef HOMR_GEOMETRIC_FUNCTIONS_HPP
#define HOMR_GEOMETRIC_FUNCTIONS_HPP

#include "./vectorclass/special/vector3d.h"

float euclideanDist(const Vec3f& p, const Vec3f& q);
Vec3f reflectVector(const Vec3f& rayN, const Vec3f& planeN);
Vec3f mirror(const Vec3f& P, const Vec3f& Q, const Vec3f& N);
Vec3f calcIntersect(const Vec3f& rayP, const Vec3f& rayN,
                    const Vec3f& polyP, const Vec3f& polyN);
bool intersectInRayDir(const Vec3f& rayP, const Vec3f& rayN, const Vec3f& iSect);

#endif // HOMR_GEOMETRIC_FUNCTIONS_HPP
