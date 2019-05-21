
#include "Geometry.hpp"

Vec8f convertAbsorbToReflectCoeffs(const Vec8f& absorbCoeffs)
{
    return -sqrt(1.0 - absorbCoeffs*absorbCoeffs);
}

Ray::Ray()
{
    _v0 = Vec3f(0.f);
    _normal = Vec3f(1.f, 0.f, 0.f);
}

Ray::Ray(const Vec3f& v0, const Vec3f& normal)
{
    _v0 = v0;
    _normal = normal;
}

Line::Line()
{
    _v1 = Vec3f(0.f);
    _normal = Vec3f(1.f, 0.f, 0.f);
    _length = 0.f;
}

Line::Line(const Vec3f& v0, const Vec3f& normal, const float length)
{
    _v0 = v0;
    _normal = normal;
    _length = length;
    _v1 = _v0 + _normal * _length;
}

Line::Line(const Vec3f& v0, const Vec3f& v1)
{
    _v0 = v0;
    _v1 = v1;
    update();
}

void Line::setStartPoint(const Vec3f& point)
{
    _v0 = point;
    update();
}

void Line::setEndPoint(const Vec3f& point)
{
    _v1 = point;
    update();
}

void Line::update()
{
    _normal = normalize_vector(_v1 - _v0);
    _length = euclideanDist(_v0, _v1);
}

Path::Path()
{
    _length = 0.f;
}

void Path::addSegment(const Line &segment)
{
    _length += segment._length;
    _segment.push_back(segment);
}

void Path::addSegment(const Vec3f& start, const Vec3f& end)
{
    Line newSegment(start, end);
    _length += newSegment._length;
    _segment.push_back(Line(start, end));
}

void Path::clear()
{
    _segment.clear();
    _length = 0.f;
}

////////////////////////////////////////////////////////////////////////////////

Polygon::Polygon()
{
    _reflectCoeffs = Vec8f(1.f);
    _diffusion = 0.f;
    _scattering = 0.f;
}

void Polygon::addVertex(const Vec3f& vertex)
{
    _vertices.push_back(vertex);
}

void Polygon::setAbsorbCoeffs(const Vec8f& coeffs)
{
    _reflectCoeffs = convertAbsorbToReflectCoeffs(coeffs);
}

Triangle::Triangle()
{
    _v0 = Vec3f(0.f);
    _v1 = Vec3f(0.f);
    _v2 = Vec3f(0.f);
    _normal = Vec3f(1.f, 0.f, 0.f);
    _u = Vec3f(0.f);
    _v = Vec3f(0.f);
    _uu = 0.f;
    _uv = 0.f;
    _vv = 0.f;
    _d = 0.f;
    _area = 0.f;
    _reflectCoeffs = Vec8f(0.f);
    _diffusion = 0.f;
    _scattering = 0.f;
}

Triangle::Triangle(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2)
{
    _v0 = v0;
    _v1 = v1;
    _v2 = v2;
    calcNormal();
    calcArea();
    _reflectCoeffs = Vec8f(0.f);
    _diffusion = 0.f;
    _scattering = 0.f;
}

void Triangle::calcNormal()
{
    _u = _v1 - _v0;
    _v = _v2 - _v0;
    _uu = dot_product(_u, _u);
    _uv = dot_product(_u, _v);
    _vv = dot_product(_v, _v);
    _d = 1.f / ((_uv * _uv) - (_uu * _vv));
    _normal = normalize_vector(cross_product(_u, _v));
}

void Triangle::calcArea()
{
    Line a(_v0, _v1);
    Line b(_v1, _v2);
    Line c(_v2, _v0);
    float s = (a._length + b._length + c._length) / 2.f;
    _area = sqrt(s * (s - a._length) * (s - b._length) * (s - c._length));
}

bool Triangle::pointInTriangle(const Vec3f& point) const
{
    Vec3f w = point - _v0;
    float wu = dot_product(w, _u);
    float wv = dot_product(w, _v);
    float s = (_uv * wv - _vv * wu) * _d;
    float t = (_uv * wu - _uu * wv) * _d;
    if(s >= 0.f && t >= 0.f && s + t <= 1.f)
    {
        return true;
    }
    return false;
}

bool Triangle::softPointInTriangle(const Vec3f &point) const
{
    Vec3f w = point - _v0;
    float wu = dot_product(w, _u);
    float wv = dot_product(w, _v);
    float s = (_uv * wv - _vv * wu) * _d;
    float t = (_uv * wu - _uu * wv) * _d;
    if(s > 0.f && t > 0.f && s + t < 1.f)
    {
        return true;
    }
    return false;
}

bool Triangle::findIntersection(const Ray &ray, Vec3f &intersection) const
{
    intersection = calcIntersect(ray._v0, ray._normal, _v0, _normal);
    return pointInTriangle(intersection);
}

std::vector<Triangle> triangulatePolygon(const Polygon& poly)
{
    std::vector<Triangle> triangles;
    if(poly._vertices.size() >= 3)
    {
        uint64_t possTriangles = poly._vertices.size() - 2;
        for(auto t = 0; t < possTriangles; ++t)
        {
            Triangle newTriangle(poly._vertices[0],
                                 poly._vertices[t + 1],
                                 poly._vertices[t + 2]);
            newTriangle._reflectCoeffs = poly._reflectCoeffs;
            newTriangle._diffusion = poly._diffusion;
            newTriangle._scattering = poly._scattering;
            triangles.push_back(newTriangle);
        }
    }
    return triangles;
}
