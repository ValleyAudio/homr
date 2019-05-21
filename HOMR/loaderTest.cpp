//
//  loaderTest.cpp
//  HOMR2
//
//  Created by Dale Johnson on 02/06/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#include "FileHandling.hpp"
#include "Model.hpp"

int main()
{
    Model model;
    loadModel(model, "room.mod");
    return 0;
}
