//
//  RayTracer.cpp
//  HOMR2
//
//  Created by Dale Johnson on 18/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#include "RayTracer.hpp"

RayTracePlan::RayTracePlan(uint32_t numRays)
{
    _infRender = true;
    _renderDepth = 0;
    _numRays = numRays;
    _ignoreDepth = 0;
}

void RayTracePlan::setInfRenderFlag(bool infRender)
{
    _infRender = infRender;
}

void RayTracePlan::setRenderDepth(uint32_t renderDepth)
{
    _infRender = false;
    _renderDepth = renderDepth;
}

void RayTracePlan::setNumRays(uint32_t numRays)
{
    _numRays = numRays;
}

void RayTracePlan::setIgnoreDepth(uint32_t ignoreDepth)
{
    _ignoreDepth = ignoreDepth;
}

bool RayTracePlan::infRender() const
{
    return _infRender;
}

uint32_t RayTracePlan::getRenderDepth() const
{
    return _renderDepth;
}

uint32_t RayTracePlan::getNumRays() const
{
    return _numRays;
}

uint32_t RayTracePlan::getIgnoreDepth() const
{
    return _ignoreDepth;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void rayTrace(const Model& model, const RayTracePlan& plan,
              RawResponse& response)
{
    std::vector<MetaRay> rays(generateRays(model._source, plan.getNumRays()));
    int surfaceID = surfaceNotFound;
    Line path;
    uint32_t outOfScope = 0;

    for(auto r = 0; r < rays.size(); ++r)
    {
        do
        {
            surfaceID = findTargetSurface(model, rays[r], path);
            if(surfaceID != surfaceNotFound)
            {
                rays[r]._surface = surfaceID;
                if(rays[r]._order > plan.getIgnoreDepth())
                {
                    detectRay(rays[r], model, path, response);
                }
                rays[r]._history.push_back(surfaceID);
                moveAndReflectRay(rays[r], model._surfaces[surfaceID], path);
                scatterAndDiffuseRay(rays[r], surfaceID, model, plan, response);
            }
            else
            {
                if(rays[r]._order > plan.getIgnoreDepth())
                {
                    detectRay(rays[r], model, response);
                }
                rays[r]._inScope = false;
            }
            ++rays[r]._order;
        }while(rayRenderable(rays[r], plan, outOfScope));
    }
}

int findTargetSurface(const Model& model, const MetaRay& ray, Line& path)
{
    int surfaceID = -1;
    Vec3f iSect;
    bool pathFound = false;
    float distance = 0.f;
    for(auto s = 0; s < model._surfaces.size(); ++s)
    {
        if(s == ray._surface)
        {
            continue;
        }
        if(model._surfaces[s].findIntersection(ray, iSect))
        {
            if(intersectInRayDir(ray._v0, ray._normal, iSect))
            {
                if(!pathFound)
                {
                    pathFound = true;
                    path = Line(ray._v0, iSect);
                }
                else
                {
                    distance = euclideanDist(ray._v0, iSect);
                    if(distance < path._length)
                    {
                        path = Line(ray._v0, iSect);
                    }
                }
                surfaceID = s;
            }
        }
    }
    return surfaceID;
}

void moveAndReflectRay(MetaRay& ray, const Triangle& surface, const Line& path)
{
    ray._v0 = path._v1;
    ray._distance += path._length;
    ray._normal = reflectVector(ray._normal, surface._normal);
    ray._energy *= surface._reflectCoeffs;
}

void detectRay(const MetaRay& ray, const Model& model,
               const Line& raySurfacePath, RawResponse& response)
{
    Line path;
    Vec3f isect;
    MetaRay rayToAdd;
    for(auto r = 0; r < model._receivers.size(); ++r)
    {
        const Receiver& rec = model._receivers[r];
        isect = calcIntersect(ray._v0, ray._normal, rec._origin, -ray._normal);
        path = Line(ray._v0, isect);
        if(path._length > raySurfacePath._length)
        {
            continue;
        }
        if(!intersectInRayDir(ray._v0, ray._normal, isect))
        {
            continue;
        }
        if(euclideanDist(isect, rec._origin) > rec._radius)
        {
            continue;
        }
        rayToAdd = ray;
        rayToAdd._distance += path._length;
        rayToAdd._energy *= inverseLaw(rayToAdd._distance);
        rayToAdd._energy *= airAbsorption(rayToAdd._distance);
        response.append(model._receivers[r]._channel, rayToAdd);
    }
}

void detectRay(const MetaRay& ray, const Model& model, RawResponse& response)
{
    Line path;
    Vec3f isect;
    MetaRay rayToAdd;
    for(auto r = 0; r < model._receivers.size(); r++)
    {
        const Receiver& rec = model._receivers[r];
        isect = calcIntersect(ray._v0, ray._normal, rec._origin, ray._normal * -1.f);
        path = Line(ray._v0, isect);
        if(!intersectInRayDir(ray._v0, ray._normal, isect))
        {
            continue;
        }
        if(euclideanDist(isect, rec._origin) > rec._radius)
        {
            continue;
        }
        rayToAdd = ray;
        rayToAdd._distance += path._length;
        rayToAdd._energy *= inverseLaw(rayToAdd._distance);
        rayToAdd._energy *= airAbsorption(rayToAdd._distance);
        response.append(model._receivers[r]._channel, rayToAdd);
    }
}

void scatterAndDiffuseRay(MetaRay& ray, int surfaceID, const Model& model, const RayTracePlan& plan,
                          RawResponse& response)
{
    /*if(ray._order < plan.getIgnoreDepth())
    {
        return;
    }*/
    if(model._surfaces[surfaceID]._diffusion == 0.f)// || ray._order < 1)
    {
        return;
    }
    float randomNumber = ((float)rand() / (float)INT_MAX);
    
    if(ray._order < 1) {
        if(fabs(randomNumber) < 1.f - 0.25f * model._surfaces[surfaceID]._diffusion) {
            return;
        }
    }
    
    if(fabs(randomNumber) < 1.f - model._surfaces[surfaceID]._diffusion)
    {
        return;
    }
    
    // Randomise ray direction
    Vec3f newRayNormal;
    Ranvec1 ran(1);
    ran.init((int)time(NULL));
    
    //float randComps[3] = {0.f, 0.f, 0.f};
    float dot = dot_product(ray._normal, model._surfaces[surfaceID]._normal);
    bool scatterValid = false;
    do
    {
        /*for(auto i = 0; i < 3; i++)
        {
            randComps[i] = ((float)rand() / (float)INT_MAX) * model._surfaces[surfaceID]._scattering;
        }*/
        Vec3f randVec = ran.random4f() * 2.f - 1.f;
        //randVec.load(randComps);
        newRayNormal = ray._normal + randVec;
        newRayNormal = normalize_vector(newRayNormal);
        
        // Check ray is on the visible side of the surface
        if(dot >= 0.f)
        {
            if(dot_product(newRayNormal, model._surfaces[surfaceID]._normal) >= 0.f)
            {
                scatterValid = true;
            }
        }
        else
        {
            if(dot_product(newRayNormal, model._surfaces[surfaceID]._normal) <= 0.f)
            {
                scatterValid = true;
            }
        }
    }while(!scatterValid);
    
    ray._normal = newRayNormal;
    
    // Spawn a child ray
    MetaRay diffRay;
    Line diffPath;
    Line occlusionPath;
    int occSurfaceID = -1;
    diffRay._diffused = true;
    
    for(auto i = 0; i < model._receivers.size(); ++i) {
        diffRay = ray;
        diffPath = Line(diffRay._v0, model._receivers[i]._origin);
        diffRay._normal = diffPath._normal;
        
        if(dot >= 0.f) {
            if(dot_product(diffRay._normal, model._surfaces[surfaceID]._normal) < 0.f) {
                continue;
            }
        }
        else {
            if(dot_product(diffRay._normal, model._surfaces[surfaceID]._normal) > 0.f) {
                continue;
            }
        }
        
        occSurfaceID = findTargetSurface(model, diffRay, occlusionPath);
        
        if(occSurfaceID != surfaceNotFound && occlusionPath._length < diffPath._length) {
            continue;
        }
        
        diffRay._distance += diffPath._length;
        diffRay._energy *= fabs(dot_product(diffRay._normal, model._surfaces[surfaceID]._normal));
        diffRay._energy *= model._surfaces[surfaceID]._scattering;
        diffRay._energy *= inverseLaw(diffRay._distance);
        diffRay._energy *= airAbsorption(diffRay._distance);
        response.append(model._receivers[i]._channel, diffRay);
    }
    
    ray._energy *= 1.f - model._surfaces[surfaceID]._scattering;
}

// Generates rays using the Fibonacci Spiral method
std::vector<MetaRay> generateRays(const Source& source, uint32_t numRays)
{
    std::vector<MetaRay> rays;
    Vec3f normal;
    float goldenAngle = M_PI * (3.f - sqrt(5.f));
    float theta = 0.f;
    float r = 0.f;
    float z = 0.f;
    float invNumRays = 1.f / ((float)numRays - 1.f);
    MetaRay newRay;
    for(float i = 0.f; i < numRays; i++)
    {
        z = i * invNumRays;
        z = z * 2.f - 1.f;
        theta = i * goldenAngle;
        r = sqrt(1.f - z * z);
        normal = Vec3f(r * cos(theta), r * sin(theta), z);
        newRay = MetaRay(source._origin, normal);
        newRay._energy = calcDirectivity(newRay._normal, source._normal,
                                         source._directivity);
        rays.push_back(newRay);
    }
    return rays;
}

bool rayRenderable(const MetaRay& ray, const RayTracePlan& plan,
                   uint32_t& outOfScopeCount)
{
    if(!ray._inScope)
    {
        return false;
    }
    if(ray._order > plan.getRenderDepth() && !plan.infRender())
    {
        return false;
    }
    if(inverseLaw(ray._distance) < EPSILON)
    {
        return false;
    }
    bool energyNegligible = true;
    for(auto i = 0; i < 8; i++)
    {
        if(fabs(ray._energy[i]) > EPSILON)
        {
            energyNegligible = false;
        }
    }
    if(energyNegligible)
    {
        return false;
    }
    return true;
}
