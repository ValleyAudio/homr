//
//  AcousticObjects.cpp
//  HOMR2
//
//  Created by Dale Johnson on 18/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#include "AcousticObjects.hpp"

MetaRay::MetaRay()
{
    _energy = Vec8f(1.f);
    _distance = 0.f;
    _inScope = true;
    _order = 0;
    _surface = -1;
    _diffused = false;
}

MetaRay::MetaRay(const Vec3f& origin, const Vec3f& normal)
       : Ray(origin, normal)
{
    _energy = Vec8f(1.f);
    _distance = 0.f;
    _inScope = true;
    _order = 0;
    _surface = -1;
    _diffused = false;
}

Source::Source()
{
    _origin = Vec3f(0.f);
}

Source::Source(const Vec3f& origin, const Vec3f& normal, const Vec8f& directivity)
{
    _origin = origin;
    _normal = normal;
    _directivity = directivity;
}

Receiver::Receiver()
{
    _origin = Vec3f(0.f);
    _normal = Vec3f(1.f, 0.f, 0.f);
    _radius = 0.f;
    _polarPattern = 1.f;
    _channel = 0;
}

Receiver::Receiver(const Vec3f& origin, const Vec3f& normal, float radius,
                   float polarPattern)
{
    _origin = origin;
    _normal = normal;
    normalize_vector(_normal);
    _radius = radius;
    _polarPattern = polarPattern;
    _channel = 0;
}

float Receiver::calcPolarMagnitude(const Vec3f &rayN)
{
    return _polarPattern + (1.f - _polarPattern) * dot_product(_normal, -1.f * rayN);
}

float inverseLaw(float distance)
{
    return 1.f / (4.f * M_PI * (distance)); // See the DAFX book
}

Vec8f airAbsorption(float distance)
{
    float x[8];
    for(auto i = 0; i < 8; ++i)
    {
        x[i] = expf(AIR_ABSORPTION[i] * distance); // H. Kuttruff equations
        //x[i] = 1.0;
    }
    return Vec8f(x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7]);
}

float calcPolarMagnitude(const Vec3f& rayN, const Vec3f& recN,
                         const float pattern)
{
    return pattern + (1.0 - pattern) * dot_product(recN, -1.0 * rayN);
}

Vec8f calcDirectivity(const Vec3f& rayN, const Vec3f& sourceN,
                      const Vec8f& pattern)
{
    return pattern + (1.0 - pattern) * dot_product(rayN, sourceN);
}
