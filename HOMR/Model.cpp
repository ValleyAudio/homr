//
//  Model.cpp
//  HOMR2
//
//  Created by Dale Johnson on 14/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#include "Model.hpp"

Model::Model()
{
    _cubicVolume = 0.f;
    _surfaceArea = 0.f;
}

void Model::addReceiver(const Receiver &newReceiver)
{
    _receivers.push_back(newReceiver);
}

void Model::addSurface(const Triangle& surface)
{
    _surfaces.push_back(surface);
}

void Model::appendSurfaces(const std::vector<Triangle> &surfaces)
{
    _surfaces.insert(_surfaces.begin(), surfaces.begin(), surfaces.end());
}

void Model::calcSurfaceArea()
{
    for(auto i = 0; i < _surfaces.size(); i++)
    {
        _surfaceArea += _surfaces[i]._area;
    }
}