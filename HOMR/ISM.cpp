//
//  ISM.cpp
//  HOMR2
//
//  Created by Dale Johnson on 25/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#include "ISM.hpp"

void ISMPlan::setDepth(unsigned int depth)
{
    _depth = depth;
}

unsigned int ISMPlan::getDepth() const
{
    return _depth;
}

Image::Image(const Vec3f& origin)
{
    _origin = origin;
    _depth = 0;
    _reflector = -1;
}

Image::Image(const Vec3f& origin, unsigned int depth)
{
    _origin = origin;
    _depth = depth;
    _reflector = -1;
}

////////////////////////////////////////////////////////////////////////////////

bool pathCrossesSurface(const Line& path, const Triangle& surface, Vec3f& isect)
{
    isect = calcIntersect(path._v0, path._normal, surface._v0, surface._normal);
    if(surface.pointInTriangle(isect) && intersectInRayDir(path._v0, path._normal, isect))
    {
        return true;
    }
    return false;
}

bool pathNotOccluded(const Model& model, const Line& path, unsigned int surfaceToIgnore)
{
    Line tempPath;
    for(auto i = 0; i < model._surfaces.size(); i++)
    {
        const Triangle& surface = model._surfaces[i];
        if(i != surfaceToIgnore)
        {
            tempPath = Line(path._v0,calcIntersect(path._v0, path._normal,
                                                   surface._v0, surface._normal));
            if(dot_product(path._normal, tempPath._normal) > 0.f)
            {
                if(tempPath._length < path._length)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

ImageTree::ImageTree(const Model& model, unsigned int depth,
                     RawResponse& response)
{
    renderDepth = depth;
    possImages = 0;
    validImages = 0;
    _root = std::shared_ptr<Image>(new Image(model._source._origin));
    for(auto r = 0; r < model._receivers.size(); r++)
    {
        Line path(_root->_origin, model._receivers[r]._origin);
        MetaRay ray(model._receivers[r]._origin, path._normal);
        ray._distance = path._length;
        ray._energy *= inverseLaw(ray._distance);
        ray._energy *= airAbsorption(ray._distance);
        response.append(r, ray);
        validImages++;
    }
    render(model, _root, response);
}

void ImageTree::render(const Model& model, std::shared_ptr<Image>& parent,
                       RawResponse& response)
{
    if(parent->_depth >= renderDepth)
    {
        return;
    }
    for(auto s = 0; s < model._surfaces.size(); s++)
    {
        const Triangle& tri = model._surfaces[s];
        std::shared_ptr<Image> img;
        img = std::shared_ptr<Image> (new Image(mirror(parent->_origin,
                                                       tri._v0,
                                                       tri._normal),
                                                parent->_depth + 1));
        possImages++;
        img->_reflector = s;
        img->_parent = parent;
        for(auto r = 0; r < model._receivers.size(); r++)
        {
            MetaRay ray(model._receivers[r]._origin,
                        normalize_vector(model._receivers[r]._origin - img->_origin));
            ray._surface = s;
            ray._order = img->_depth;
            if(validate(model, model._receivers[r]._origin, img, ray))
            {
                ray._energy *= inverseLaw(ray._distance);
                ray._energy *= airAbsorption(ray._distance);
                response.append(r, ray);
            }
        }
        render(model, img, response);
    }
}

bool ImageTree::validate(const Model& model, const Vec3f& startPoint,
                         std::shared_ptr<Image>& image, MetaRay& ray)
{
    Line imagePath(startPoint, image->_origin);
    if(image->_depth == 0)
    {
        ray._distance += imagePath._length;
        validImages++;
        return true;
    }
    Vec3f isect = calcIntersect(imagePath._v0, imagePath._normal,
                                model._surfaces[image->_reflector]._v0,
                                model._surfaces[image->_reflector]._normal);
    Line realPath(startPoint, isect);
    if(model._surfaces[image->_reflector].pointInTriangle(isect)
       && realPath._length <= imagePath._length
       && pathNotOccluded(model, realPath, image->_reflector))
    {
        ray._distance += realPath._length;
        ray._energy *= model._surfaces[image->_reflector]._reflectCoeffs;
        return validate(model, isect, image->_parent, ray);
    }
    return false;
}

void ISM(const Model& model, const ISMPlan& plan, RawResponse& response)
{
    std::vector<std::vector<Image>> _images;
    _images.resize(plan.getDepth() + 1);
    _images[0].push_back(Image(model._source._origin, 0));
    for(auto i = 0; i < plan.getDepth(); i++)
    {
        printf("%d\n", i);
        for(auto j = 0; j < _images[i].size(); j++)
        {
            //printf("  %d\n", j);
            for(auto k = 0; k < model._surfaces.size(); k++)
            {
                const Triangle& surface = model._surfaces[k];
                _images[i + 1].push_back(Image(mirror(_images[i][j]._origin, surface._v0, surface._normal), i + 1));
            }
        }
    }
}
