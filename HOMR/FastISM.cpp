//
//  FastISM.cpp
//  HOMR2
//
//  Created by Dale Johnson on 23/08/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#include "FastISM.hpp"

Image::Image(const Vec3f& origin)
{
    _origin = origin;
    _reflector = -1;
    _depth = 0;
}

Image::Image(const Vec3f& origin, std::shared_ptr<Image>& parent, const int reflector, const unsigned int depth)
{
    _origin = origin;
    _reflector = reflector;
    _depth = depth;
    _parent = parent;
}

void ism(const Model& model, RawResponse& response, unsigned int maxDepth)
{
    std::shared_ptr<Image> root = std::shared_ptr<Image>(new Image(model._source._origin));
    
    // Attempt to create direct sound paths from root image to receivers
    for(auto i = 0; i < model._receivers.size(); ++i)
    {
        Line directPath = Line(model._source._origin, model._receivers[i]._origin);
        if(segmentValid(model, directPath, root))
        {
            MetaRay directSound = MetaRay(directPath._v0, directPath._normal);
            directSound._distance = directPath._length;
            directSound._energy *= inverseLaw(directSound._distance);
            directSound._energy *= airAbsorption(directSound._distance);
            response.append(i, directSound);
        }
    }
    render(model, response, root, maxDepth);
}

void render(const Model& model, RawResponse& response, std::shared_ptr<Image>& parent, unsigned int maxDepth)
{
    if(parent->_depth >= maxDepth)
    {
        return;
    }
    std::shared_ptr<Image> image;
    for(auto i = 0; i < model._surfaces.size(); ++i)
    {
        if(i != parent->_reflector)
        {
            image = std::shared_ptr<Image>(new Image(mirror(parent->_origin, model._surfaces[i]._v0,
                                                            model._surfaces[i]._normal),
                                                            parent, i, parent->_depth + 1));
            backTrace(model, response, image);
            render(model, response, image, maxDepth); // Call this function 1 layer deeper
        }
    }
}

void backTrace(const Model& model, RawResponse& response, std::shared_ptr<Image>& image)
{
    Line imagePath; // From image to image
    Line segment; // From image to surface intersection
    Path path;
    Vec3f iSect;
    std::shared_ptr<Image> endImage;
    std::vector<int> surfacesToIgnore;
    std::vector<int> surfaceOrder;
    for(auto r = 0; r < model._receivers.size(); ++r)
    {
        endImage = image;
        surfacesToIgnore.clear();
        surfaceOrder.clear();
        path.clear();

        // 1. Receiver to image
        imagePath = Line(model._receivers[r]._origin, endImage->_origin);
        model._surfaces[endImage->_reflector].findIntersection(imagePath, iSect);
        segment = Line(imagePath._v0, iSect);
        surfacesToIgnore.push_back(endImage->_reflector);
        if(!segmentValid(model, segment, imagePath, endImage, surfacesToIgnore))
        {
            continue;
        }
        surfaceOrder.push_back(endImage->_reflector);
        path.addSegment(segment);

        // Append image reflector and next image reflector to surfaces to ignore
        surfacesToIgnore.clear();
        surfacesToIgnore.push_back(endImage->_reflector);
        endImage = endImage->_parent;
        surfacesToIgnore.push_back(endImage->_reflector);

        // 2. Image to image
        bool pathValid = true;
        while(endImage->_depth != 0)
        {
            surfacesToIgnore.clear();
            imagePath = Line(segment._v1, endImage->_origin);
            model._surfaces[endImage->_reflector].findIntersection(imagePath, iSect);
            segment = Line(imagePath._v0, iSect);
            if(segment._length == 0.f)
            {
                pathValid = false;
                break;
            }
            if(!segmentValid(model, segment, imagePath, endImage, surfacesToIgnore))
            {
                pathValid = false;
                break;
            }

            surfaceOrder.push_back(endImage->_reflector);
            path.addSegment(segment);

            // Append image reflector and next image reflector to surfaces to ignore
            surfacesToIgnore.clear();
            surfacesToIgnore.push_back(endImage->_reflector);
            endImage = endImage->_parent;
            surfacesToIgnore.push_back(endImage->_reflector);
        }
        if(!pathValid)
        {
            continue;
        }

        // 3. Image to source
        segment = Line(segment._v1, endImage->_origin);
        if(!segmentValid(model, segment, endImage, surfacesToIgnore))
        {
            continue;
        }
        path.addSegment(segment);

        MetaRay virtualPath = MetaRay(path._segment[0]._v1, -path._segment[0]._normal);
        virtualPath._distance = path._length;
        virtualPath._energy *= inverseLaw(virtualPath._distance);
        virtualPath._energy *= airAbsorption(virtualPath._distance);
        virtualPath._order = image->_depth;
        for(auto i = 0; i < surfaceOrder.size(); ++i)
        {
            virtualPath._history.push_back(surfaceOrder[i]);
            virtualPath._energy *= model._surfaces[surfaceOrder[i]]._reflectCoeffs;
        }
        std::reverse(virtualPath._history.begin(), virtualPath._history.end());
        virtualPath._surface = virtualPath._history[0];
        response.append(model._receivers[r]._channel, virtualPath);
    }
}

bool segmentValid(const Model& model, const Line& segment, const Line& imagePath,
                  std::shared_ptr<Image>& image, const std::vector<int>& surfacesToIgnore)
{
    if(isnan(segment._length) || isnan(imagePath._length)) // Infinity check
    {
        return false;
    }
    if(segment._length > imagePath._length) // Is image behind reflector
    {
        return false;
    }
    // Are the paths in the same direction
    if(dot_product(segment._normal, imagePath._normal) < 0.98f)
    {
        return false;
    }
    // Does path cross reflector
    if(!model._surfaces[image->_reflector].pointInTriangle(segment._v1))
    {
        return false;
    }

    // Occlusion Test
    Vec3f iSect;
    Line occlusionPath = segment;
    for(auto s = 0; s < model._surfaces.size(); ++s)
    {
        bool ignoreSurface = false;
        for(auto i = 0; i < surfacesToIgnore.size(); ++i) // Check ignore surfaces list
        {
            if(s == surfacesToIgnore[i])
            {
                ignoreSurface = true;
                break;
            }
        }
        if(ignoreSurface)
        {
            continue;
        }
        model._surfaces[s].findIntersection(segment, iSect);
        occlusionPath.setEndPoint(iSect);
        
        // Negates false occlusion bug which occurs at polygon edges where paths can
        // get 'stuck' or penetrate through surface hulls due to the hard occlusion method
        // used in the Triangle object. If the length is 0, it means it's caught by a neighbouring
        // polygon, which will reflect the path back again and cause it to penetrate the hull.
        if(occlusionPath._length >= segment._length || occlusionPath._length == 0.f)
        {
            continue;
        }
        
        // Occlusions have to occur in direction of segment
        if(dot_product(occlusionPath._normal, segment._normal) < 0.98f)
        {
            continue;
        }
        if(model._surfaces[s].pointInTriangle(iSect))
        {
            return false;
        }
    }
    return true;
}

bool segmentValid(const Model& model, const Line& segment, std::shared_ptr<Image>& image,
                  const std::vector<int>& surfacesToIgnore)
{
    if(isnan(segment._length))
    {
        return false;
    }

    // Occlusion Test
    Vec3f iSect;
    Line occlusionPath = segment;
    for(auto s = 0; s < model._surfaces.size(); ++s)
    {
        bool ignoreSurface = false;
        for(auto i = 0; i < surfacesToIgnore.size(); ++i)
        {
            if(s == surfacesToIgnore[i])
            {
                ignoreSurface = true;
                break;
            }
        }
        if(ignoreSurface)
        {
            continue;
        }
        model._surfaces[s].findIntersection(segment, iSect);
        occlusionPath.setEndPoint(iSect);
        if(occlusionPath._length >= segment._length)
        {
            continue;
        }
        if(dot_product(occlusionPath._normal, segment._normal) < 0.98f)
        {
            continue;
        }
        if(model._surfaces[s].pointInTriangle(iSect) && occlusionPath._length > 0.f)
        {
            return false;
        }
    }
    return true;
}

bool segmentValid(const Model& model, const Line& segment, std::shared_ptr<Image>& image)
{
    if(isnan(segment._length))
    {
        return false;
    }

    // Occlusion Test
    Vec3f iSect;
    Line occlusionPath = segment;
    for(auto s = 0; s < model._surfaces.size(); ++s)
    {
        model._surfaces[s].findIntersection(segment, iSect);
        occlusionPath.setEndPoint(iSect);
        if(occlusionPath._length >= segment._length)
        {
            continue;
        }
        if(dot_product(occlusionPath._normal, segment._normal) < 0.98f)
        {
            continue;
        }
        if(model._surfaces[s].pointInTriangle(iSect) && occlusionPath._length > 0.f)
        {
            return false;
        }
    }
    return true;
}
