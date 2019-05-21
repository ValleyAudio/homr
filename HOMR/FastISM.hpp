//
//  FastISM.hpp
//  HOMR2
//
//  Created by Dale Johnson on 23/08/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#ifndef HOMR_FAST_ISM_HPP
#define HOMR_FAST_ISM_HPP

#include "AcousticObjects.hpp"
#include "Model.hpp"
#include "ImpulseResponse.hpp"
#include <memory>
#include <algorithm>

struct Image
{
    Image(const Vec3f& origin);
    Image(const Vec3f& origin, std::shared_ptr<Image>& parent, const int reflector, const unsigned int depth);
    Vec3f _origin;
    int _reflector;
    unsigned int _depth;
    std::shared_ptr<Image> _parent;
};

void ism(const Model& model, RawResponse& response, unsigned int maxDepth);
void render(const Model& model, RawResponse& response, std::shared_ptr<Image>& parent, unsigned int maxDepth);
void backTrace(const Model& model, RawResponse& response, std::shared_ptr<Image>& image);
bool segmentValid(const Model& model, const Line& segment, const Line& imagePath,
                  std::shared_ptr<Image>& image, const std::vector<int>& surfacesToIgnore);
bool segmentValid(const Model& model, const Line& segment, std::shared_ptr<Image>& image,
                  const std::vector<int>& surfacesToIgnore);
bool segmentValid(const Model& model, const Line& segment, std::shared_ptr<Image>& image);

#endif // HOMR_FAST_ISM_HPP
