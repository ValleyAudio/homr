//
//  ISM.hpp
//  HOMR2
//
//  Created by Dale Johnson on 25/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#ifndef HOMR_ISM_HPP
#define HOMR_ISM_HPP

#include "Model.hpp"
#include "AcousticObjects.hpp"
#include "Geometry.hpp"
#include "ImpulseResponse.hpp"

class ISMPlan
{
public:
    void setDepth(unsigned int depth);
    unsigned int getDepth() const;
private:
    unsigned int _depth;
};

struct Image
{
    Image(const Vec3f& origin);
    Image(const Vec3f& origin, const unsigned int depth);
    int _reflector;
    Vec3f _origin;
    unsigned int _depth;
    std::shared_ptr<Image> _parent;
    
};

////////////////////////////////////////////////////////////////////////////////

bool pathCrossesSurface(const Line& path, const Triangle& surface, Vec3f& iSect);
bool pathOccluded(const Model& model, const Line& path, unsigned int surfaceToIgnore);

class ImageTree
{
public:
    ImageTree(const Model& model, unsigned int depth, RawResponse& response);
private:
    unsigned int renderDepth;
    unsigned int possImages;
    unsigned int validImages;
    std::shared_ptr<Image> _root;
    void render(const Model& model, std::shared_ptr<Image>& parent,
                RawResponse& response);
    bool validate(const Model& model, const Vec3f& startPoint,
                  std::shared_ptr<Image>& image, MetaRay& ray);
};

void ISM(const Model& model, const ISMPlan& plan, RawResponse& response);

#endif /* HOMR_ISM_HPP */
