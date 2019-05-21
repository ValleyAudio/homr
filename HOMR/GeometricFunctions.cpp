//
//  GeometricFunctions.cpp
//
//  Created by Dale Johnson on 14/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#include "GeometricFunctions.hpp"

float euclideanDist(const Vec3f& p, const Vec3f& q)
{
    return sqrt(horizontal_add((q - p) * (q - p)));
}

Vec3f reflectVector(const Vec3f& rayN, const Vec3f& planeN)
{
    return normalize_vector(rayN - (planeN * 2.f * dot_product(rayN, planeN)));
}

Vec3f mirror(const Vec3f& P, const Vec3f& Q, const Vec3f& N)
{
    return ((P - Q) - 2.f * dot_product(N, P - Q) * N) + Q;
}

Vec3f calcIntersect(const Vec3f& rayP, const Vec3f& rayN,
                    const Vec3f& polyP, const Vec3f& polyN)
{
    return rayN * (dot_product(polyN, (polyP - rayP)) / dot_product(rayN, polyN)) + rayP;
}

bool intersectInRayDir(const Vec3f& rayP, const Vec3f& rayN, const Vec3f& iSect)
{
    if(dot_product(rayN, normalize_vector(iSect - rayP)) > 0.98f)
    {
        return true;
    }
    return false;
}
