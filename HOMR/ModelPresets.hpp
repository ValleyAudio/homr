//
//  ModelPresets.hpp
//  HOMR2
//
//  Created by Dale Johnson on 29/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#ifndef ModelPresets_hpp
#define ModelPresets_hpp

#include "Model.hpp"

Model rectangularHall(float height, float width, float depth,
                      const Vec8f& leftWallAbsorb, const Vec8f& frontWallAbsorb,
                      const Vec8f& rightWallAbsorb, const Vec8f& rearWallAbsorb,
                      const Vec8f& floorAbsorb, const Vec8f& ceilAbsorb);

#endif /* ModelPresets_hpp */
