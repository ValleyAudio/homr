//
//  Model.hpp
//  HOMR2
//
//  Created by Dale Johnson on 14/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#ifndef Model_hpp
#define Model_hpp

#include "AcousticObjects.hpp"

struct Model
{
    Model();
    Source _source;
    std::vector<Receiver> _receivers;
    std::vector<Triangle> _surfaces;
    float _cubicVolume;
    float _surfaceArea;
    void addReceiver(const Receiver& newReceiver);
    void addSurface(const Triangle& surface);
    void appendSurfaces(const std::vector<Triangle>& surfaces);
    void calcSurfaceArea();
};

#endif /* Model_hpp */
