//
//  RayTracer.hpp
//  HOMR2
//
//  Created by Dale Johnson on 18/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#ifndef RayTracer_hpp
#define RayTracer_hpp

#include "Model.hpp"
#include "AcousticObjects.hpp"
#include "ImpulseResponse.hpp"
#include <random>
#include <time.h>

static const int surfaceNotFound = -1;

class RayTracePlan
{
public:
    RayTracePlan(uint32_t numRays);
    void setInfRenderFlag(bool infRender);
    void setRenderDepth(uint32_t renderDepth);
    void setNumRays(uint32_t numRays);
    void setIgnoreDepth(uint32_t ignoreDepth);
    bool infRender() const;
    uint32_t getRenderDepth() const;
    uint32_t getNumRays() const;
    uint32_t getIgnoreDepth() const;
private:
    bool _infRender;
    uint32_t _renderDepth;
    uint32_t _numRays;
    uint32_t _ignoreDepth;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void rayTrace(const Model& model, const RayTracePlan& plan, RawResponse& response);
int findTargetSurface(const Model& model, const MetaRay& ray, Line& path);
void moveAndReflectRay(MetaRay& ray, const Triangle& surface, const Line& path);
void detectRay(const MetaRay& ray, const Model& model, const Line& raySurfacePath,
               RawResponse& response);
void detectRay(const MetaRay& ray, const Model& model, RawResponse& response);
void scatterAndDiffuseRay(MetaRay& ray, int surfaceID, const Model& model, const RayTracePlan& plan,
                          RawResponse& response);
std::vector<MetaRay> generateRays(const Source& source, uint32_t numRays);
bool rayRenderable(const MetaRay& ray, const RayTracePlan& plan,
                   uint32_t& outOfScopeCount);


#endif /* RayTracer_hpp */
