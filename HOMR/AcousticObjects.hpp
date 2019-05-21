//
//  AcousticObjects.hpp
//  HOMR2
//
//  Created by Dale Johnson on 18/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#ifndef HOMR_ACOUSTIC_OBJECTS
#define HOMR_ACOUSTIC_OBJECTS
#define EPSILON 0.00009F
#define SPEED_OF_SOUND 343.F
#define TIME_OF_SOUND 1.F / SPEED_OF_SOUND
#define USE_MATH_DEFINES
#include <math.h>
#include "Geometry.hpp"

const float POLAR_PATTERN_OMNI = 1.f;
const float POLAR_PATTERN_SUB_CARDIOID = 0.75f;
const float POLAR_PATTERN_CARDIOID = 0.5f;
const float POLAR_PATTERN_SUPER_CARDIOID = 0.33333f;
const float POLAR_PATTERN_HYPER_CARDIOID = 0.16666f;
const float POLAR_PATTERN_FIGURE_OF_EIGHT = 0.f;
const float AIR_ABSORPTION[8] = {-0.0001f, -0.0003f, -0.0006f, -0.001f,
                                 -0.0017f, -0.0041f, -0.0135f, -0.0139f}; // Room temperature

/*const float AIR_ABSORPTION[8] = {-0.0001f, -0.0003f, -0.0006f, -0.001f,
                                 -0.0017f, -0.0035f, -0.0106f, -0.0112f};*/

struct MetaRay : public Ray
{
    MetaRay();
    MetaRay(const Vec3f& origin, const Vec3f& normal);
    Vec8f _energy;
    float _distance;
    bool _inScope;
    uint32_t _order;
    int _surface;
    std::vector<int> _history;
    bool _diffused;
};

struct Source
{
    Source();
    Source(const Vec3f& origin, const Vec3f& normal, const Vec8f& directivity);
    Vec3f _origin;
    Vec3f _normal;
    Vec8f _directivity;
};

struct Receiver
{
    Receiver();
    Receiver(const Vec3f& origin, const Vec3f& normal, float radius, float polarPattern);
    Vec3f _origin;
    Vec3f _normal;
    float _radius;
    float _polarPattern;
    uint32_t _channel;
    float calcPolarMagnitude(const Vec3f& rayN);
};

float inverseLaw(float distance);
Vec8f airAbsorption(float distance);
float calcPolarMagnitude(const Vec3f& rayN, const Vec3f& recN,
                         const float pattern);
Vec8f calcPolarMagnitude(const Vec3f& rayN, const Vec3f& recN,
                         const Vec8f& pattern);
Vec8f calcDirectivity(const Vec3f& rayN, const Vec3f& normal,
                      const Vec8f& pattern);

#endif /* AcousticObjects_hpp */
