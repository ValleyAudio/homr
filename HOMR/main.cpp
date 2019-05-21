//
//  main.cpp
//  HOMR2
//
//  Created by Dale Johnson on 12/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#include "RayTracer.hpp"
#include "FastISM.hpp"
#include "CppWav.h"
#include "ModelPresets.hpp"
#include "FileHandling.hpp"
#include <iostream>

struct mainArgList
{
    std::string modelFilename;
    std::string configFilename;
    unsigned int ismDepth;
    unsigned int rayCount;
    float sampleRate;
    std::string outputFilename;
    bool normalise;
};

bool parseArgs(int argc, char* argv[], mainArgList& argList)
{
    int expectedArgs = 7;
    int argsFound = 0;
    int currentArg = 1;
    int param = 0;
    while(currentArg < argc)
    {
        param = currentArg + 1;
        if(param >= argc)
        {
            return false;
        }
        if(strncmp(argv[currentArg], "-m", 2) == 0 )
        {
            argList.modelFilename = argv[param];
        }
        else if(strncmp(argv[currentArg], "-c", 2) == 0)
        {
            argList.configFilename = argv[param];
        }
        else if(strncmp(argv[currentArg], "-i", 2) == 0)
        {
            argList.ismDepth = std::atoi(argv[param]);
        }
        else if(strncmp(argv[currentArg], "-r", 2) == 0)
        {
            argList.rayCount = std::atoi(argv[param]);
        }
        else if(strncmp(argv[currentArg], "-s", 2) == 0)
        {
            argList.sampleRate = std::atof(argv[param]);
        }
        else if(strncmp(argv[currentArg], "-n", 2) == 0)
        {
            argList.outputFilename = argv[param];
        }
        else if(strncmp(argv[currentArg], "-l", 2) == 0)
        {
            if(std::atoi(argv[param]) > 0)
            {
                argList.normalise = true;
            }
            else
            {
                argList.normalise = false;
            }
        }
        else
        {
            return false;
        }
        argsFound++;
        currentArg += 2;
    }
    if(argsFound == expectedArgs)
    {
        return true;
    }
    return false;
}

void displayHelp()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "    -m, Model filename" << std::endl;
    std::cout << "    -c, Config filename" << std::endl;
    std::cout << "    -i, ISM depth" << std::endl;
    std::cout << "    -r, Ray count" << std::endl;
    std::cout << "    -s, Sample rate" << std::endl;
    std::cout << "    -n, Output filename" << std::endl;
    std::cout << "    -l, Normalise flag" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// Main ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    std::cout << std::endl;
    std::cout << "Hall Of Mirrors Reverb" << std::endl;
    std::cout << "Copyright Dale Johnson (c) 2016" << std::endl;
    std::cout << std::endl;
    mainArgList argList;
    if(!parseArgs(argc, argv, argList))
    {
        displayHelp();
        return 0;
    }
    Model model;
    
    std::cout << "Rendering model file '" << argList.modelFilename << "'" << std::endl;
    std::cout << "Config file '" << argList.configFilename << "'" << std::endl;
    if(!loadModel2(model, argList.modelFilename))
    {
        printf("Error loading model file!\n");
        return 0;
    }
    if(!loadConfig(model, argList.configFilename))
    {
        printf("Error loading configuration file!\n");
        return 0;
    }
    
    std::cout << "Rendering room '" << argList.modelFilename << "' ";
    std::cout << "using config '" << argList.configFilename << "', ";
    std::cout << "ISM order: " << argList.ismDepth;
    std::cout << " ray count: " << argList.rayCount << std::endl;
    std::cout << std::endl;
    
    std::string fileName = argList.outputFilename;
    std::string rivFilename = fileName;
    RayTracePlan rayTracePlan(argList.rayCount);
    fileName += ".wav";

    rayTracePlan.setIgnoreDepth(argList.ismDepth);
    RawResponse response(model._receivers.size());
    
    printf("Performing ISM...\n");
    ism(model, response, argList.ismDepth);
    printf("ISM complete.\n");
    
    printf("Ray tracing...\n");
    rayTrace(model, rayTracePlan, response);
    printf("Ray tracing complete.\n");
    for(auto i = 0; i < response.getNumChannels(); ++i)
    {
        printf("Receiver %d detected %ld.\n", i, response.getChannelData(i).size());
    }
    
    if(argList.normalise)
    {
        printf("Normalising response...\n");
        normaliseRawResponse(response);
        printf("Normalisation complete.\n");
    }
    
    printf("Exporting response...\n");
    exportRawResponse(response, rivFilename);
    printf("Export complete.\n");
    
    printf("Converting RAW to IR...\n");
    ImpulseResponse ir = convertRawToIR(response, model, argList.sampleRate, extractEntireRaw());
    printf("Conversion complete.\n");
    
    printf("Saving as WAV file...\n");
    CppWav wav = convertIRToWav(ir, argList.sampleRate, WAV_24_BIT);
    wav.saveFile(fileName.data());
    printf("Save complete.\n");

    return 0;
}
