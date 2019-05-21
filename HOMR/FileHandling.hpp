//
//  FileHandling.hpp
//  HOMR2
//
//  Created by Dale Johnson on 27/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#ifndef HOMR_FILE_HANDLING_HPP
#define HOMR_FILE_HANDLING_HPP
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "Model.hpp"

float loadFloat(const std::string& line);
bool loadVec3f(const std::string& line, Vec3f& destVec);
bool loadVec8f(const std::string& line, Vec8f& destVec);

////////////////////////////////////////////////////////////////////////////////

enum modelCommand
{
    MODEL_START = 0,
    MODEL_LOAD_SURFACE,
    MODEL_END,
    MODEL_INVALID,
    MODEL_COMMENT
};
modelCommand hashModelCommand(const std::string& command);

const unsigned int MODEL_SURFACE_MIN_VERTICES = 3;
const unsigned int MODEL_SURFACE_MIN_PARAMS = 3;
enum modelSurfaceParam
{
    MODEL_SURFACE_VTX = 0,
    MODEL_SURFACE_ABSORB,
    MODEL_SURFACE_DIFFUSION,
    MODEL_SURFACE_SCATTERING,
    MODEL_SURFACE_END,
    MODEL_SURFACE_COMMENT,
    MODEL_SURFACE_INVALID
};
modelSurfaceParam hashModelSurfaceParam(const std::string& param);

bool loadModel(Model& destModel, const std::string& fileName);
bool loadSurface(Model& destModel, std::ifstream& file);

bool loadModel2(Model& destModel, const std::string& filename);
bool loadSurface2(Model& destModel, std::ifstream& file, uint64_t& lineCounter);

////////////////////////////////////////////////////////////////////////////////

enum configLoaderCommand
{
    CONFIG_START = 0,
    CONFIG_LOAD_SOURCE,
    CONFIG_LOAD_RECEIVER,
    CONFIG_END,
    CONFIG_INVALID
};
configLoaderCommand hashConfigCommand(const std::string& command);

const unsigned int PARAMS_SOURCE = 3;
enum loadSourceParams
{
    LOAD_SOURCE_ORIGIN = 0,
    LOAD_SOURCE_NORMAL,
    LOAD_SOURCE_DIR,
    LOAD_SOURCE_END,
    LOAD_SOURCE_INVALID
};
//loadSourceParams hashLoadSourceParam(const std::string& param);

const unsigned int PARAMS_RECEIVER = 5;
enum loadReceiverParams
{
    LOAD_RECEIVER_CHANNEL = 0,
    LOAD_RECEIVER_ORIGIN,
    LOAD_RECEIVER_NORMAL,
    LOAD_RECEIVER_PATTERN,
    LOAD_RECEIVER_RADIUS,
    LOAD_RECEIVER_END,
    LOAD_RECEIVER_INVALID
};
loadReceiverParams hashLoadReceiverParam(const std::string& param);
float hashPolarPattern(const std::string& line);

bool loadConfig(Model& destModel, const std::string& fileName);
bool loadSource(Model& destModel, std::ifstream& file);
bool loadReceiver(Model& destModel, std::ifstream& file);

#endif // HOMR_FILE_HANDLING_HPP
