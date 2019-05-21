//
//  Geometry.hpp
//
//  Created by Dale Johnson on 12/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#ifndef HOMR_GEOMETRY_HPP
#define HOMR_GEOMETRY_HPP

#include <vector>
#include <cstdint>
#include "./vectorclass/special/vector3d.h"
#include "./vectorclass/special/ranvec1.h"
#include "GeometricFunctions.hpp"

////////////////////////////////////////////////////////////////////////////////
// Absorption Coefficients (62Hz, 125Hz, 250Hz, 500Hz, 1kHz, 2kHz, 4kHz, 8kHz)
////////////////////////////////////////////////////////////////////////////////

const Vec8f ABSORB_CEILING_PLASTER_SMOOTH = Vec8f(0.013f, 0.013f, 0.015f, 0.02f, 0.03f, 0.04f, 0.05f, 0.05f);
const Vec8f ABSORB_CEILING_PLASTER_ROUGH = Vec8f(0.02f, 0.02f, 0.03f, 0.04f, 0.05f, 0.04f, 0.03f, 0.03f);
const Vec8f ABSORB_CEILING_PLYWOOD = Vec8f(0.28f, 0.28f, 0.22f, 0.17f, 0.09f, 0.10f, 0.11f, 0.11f);
const Vec8f ABSORB_CEILING_ACOUSTIC_TILES = Vec8f(0.70, 0.70, 0.66, 0.72, 0.92, 0.88, 0.75, 0.75);
const Vec8f ABSORB_CEILING_PLASTERBOARD = Vec8f(0.20, 0.20, 0.14, 0.10, 0.08, 0.04, 0.02, 0.02);

const Vec8f ABSORB_FLOOR_CARPET_ON_CONCRETE = Vec8f(0.02, 0.02, 0.06, 0.14, 0.37, 0.60, 0.65, 0.65);
const Vec8f ABSORB_FLOOR_CARPET_ON_FOAM = Vec8f(0.08, 0.08, 0.24, 0.57, 0.69, 0.71, 0.73, 0.73);
const Vec8f ABSORB_FLOOR_CONCRETE_COARSE = Vec8f(0.02, 0.02, 0.03, 0.03, 0.03, 0.04, 0.07, 0.07);
const Vec8f ABSORB_FLOOR_CONCRETE = Vec8f(0.01, 0.01, 0.01, 0.015, 0.02, 0.02, 0.02, 0.02);
const Vec8f ABSORB_FLOOR_LINO = Vec8f(0.02, 0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02);
const Vec8f ABSORB_FLOOR_MARBLE = Vec8f(0.01, 0.01, 0.01, 0.01, 0.02, 0.02, 0.02, 0.02);
const Vec8f ABSORB_FLOOR_PARQUET = Vec8f(0.04, 0.04, 0.04, 0.07, 0.06, 0.06, 0.07, 0.07);
const Vec8f ABSORB_FLOOR_WOOD = Vec8f(0.15, 0.15, 0.11, 0.10, 0.07, 0.06, 0.07, 0.07);

const Vec8f ABSORB_SEATING_EMPTY_FABRIC = Vec8f(0.49, 0.49, 0.66, 0.80, 0.88, 0.82, 0.70, 0.70);
const Vec8f ABSORB_SEATING_EMPTY_WOOD = Vec8f(0.15, 0.15, 0.19, 0.22, 0.39, 0.38, 0.30, 0.30);
const Vec8f ABSORB_SEATING_FULL_FABRIC = Vec8f(0.60, 0.60, 0.74, 0.88, 0.96, 0.93, 0.85, 0.85);
const Vec8f ABSORB_SEATING_FULL_WOOD = Vec8f(0.57, 0.57, 0.61, 0.75, 0.86, 0.91, 0.86, 0.86);

const Vec8f ABSORB_WALL_BRICK_UNGLAZED = Vec8f(0.03, 0.03, 0.03, 0.03, 0.04, 0.05, 0.07, 0.07);
const Vec8f ABSORB_WALL_BRICK_PAINTED = Vec8f(0.01, 0.01, 0.01, 0.02, 0.02, 0.02, 0.03, 0.03);
const Vec8f ABSORB_WALL_CONCRETE_COARSE = Vec8f(0.36, 0.36, 0.44, 0.31, 0.29, 0.39, 0.25, 0.25);
const Vec8f ABSORB_WALL_CONCRETE_PAINTED = Vec8f(0.10, 0.10, 0.05, 0.06, 0.07, 0.09, 0.08, 0.08);
const Vec8f ABSORB_WALL_GLASS_WINDOW = Vec8f(0.35, 0.35, 0.25, 0.18, 0.12, 0.07, 0.04, 0.04);
const Vec8f ABSORB_WALL_PLASTER_ROUGH = Vec8f(0.02, 0.02, 0.03, 0.04, 0.05, 0.04, 0.03, 0.03);
const Vec8f ABSORB_WALL_PLASTER_SMOOTH = Vec8f(0.013, 0.013, 0.015, 0.02, 0.03, 0.04, 0.05, 0.05);
const Vec8f ABSORB_WALL_PLYWOOD = Vec8f(0.28, 0.28, 0.22, 0.17, 0.09, 0.10, 0.11, 0.11);

Vec8f convertAbsorbToReflectCoeffs(const Vec8f& absorbCoeffs);

////////////////////////////////////////////////////////////////////////////////
// Primitives
////////////////////////////////////////////////////////////////////////////////

struct Ray
{
    Ray();
    Ray(const Vec3f& v0, const Vec3f& normal);
    Vec3f _v0;
    Vec3f _normal;
};

struct Line : public Ray
{
    Line();
    Line(const Vec3f& v0, const Vec3f& normal, const float length);
    Line(const Vec3f& v0, const Vec3f& v1);
    void setStartPoint(const Vec3f& point);
    void setEndPoint(const Vec3f& point);
    void update();
    Vec3f _v1;
    float _length;
};

struct Path
{
    Path();
    void addSegment(const Line& segment);
    void addSegment(const Vec3f& start, const Vec3f& end);
    void clear();
    std::vector<Line> _segment;
    float _length;
};

////////////////////////////////////////////////////////////////////////////////
// Polygons
////////////////////////////////////////////////////////////////////////////////

struct Polygon
{
    Polygon();
    void addVertex(const Vec3f& vertex);
    void setAbsorbCoeffs(const Vec8f& coeffs);
    std::vector<Vec3f> _vertices;
    Vec8f _reflectCoeffs;
    double _diffusion;
    double _scattering;
};

struct Triangle
{
    Triangle();
    Triangle(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2);
    void calcNormal();
    void calcArea();
    bool pointInTriangle(const Vec3f& point) const;
    bool softPointInTriangle(const Vec3f& point) const;
    bool findIntersection(const Ray& ray, Vec3f& intersection) const;
    Vec3f _v0;
    Vec3f _v1;
    Vec3f _v2;
    Vec3f _normal;
    Vec3f _u;
    Vec3f _v;
    float _uu;
    float _uv;
    float _vv;
    float _d;
    float _area;
    Vec8f _reflectCoeffs;
    float _diffusion;
    float _scattering;
};

std::vector<Triangle> triangulatePolygon(const Polygon& poly);

#endif // HOMR_GEOMETRY_HPP
