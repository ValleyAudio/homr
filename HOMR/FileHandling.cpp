//
//  FileHandling.cpp
//  HOMR2
//
//  Created by Dale Johnson on 27/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#include "FileHandling.hpp"

float loadFloat(const std::string& line)
{
    auto pos = line.find(':');
    if(pos == std::string::npos)
    {
        return 0;
    }
    auto cPos = pos + 1;
    std::string word;
    float number;
    while(cPos < line.size())
    {
        if(line[cPos] != ' ' && line[cPos] != ';')
        {
            word += line[cPos];
        }
        else if(line[cPos] == ';')
        {
            break;
        }
        cPos++;
    }
    number = std::stof(word);
    return number;
}

bool loadVec3f(const std::string& line, Vec3f& destVec)
{
    float vec[3] = {0.f, 0.f, 0.f};
    auto openPos = line.find('[');
    auto closePos = line.find(']', openPos + 1);
    if(openPos == std::string::npos || closePos == std::string::npos)
    {
        return false;
    }
    auto cpos = openPos + 1;
    auto comp = 0;
    std::string word;
    while(cpos < closePos)
    {
        word.clear();
        while(line[cpos] != ',' && cpos < closePos)
        {
            if(line[cpos] != ' ')
            {
                word += line[cpos];
            }
            cpos++;
        }
        vec[comp] = std::stof(word);
        comp++;
        cpos++;
    }
    destVec.load(vec);
    return true;
}

bool loadVec8f(const std::string& line, Vec8f& destVec)
{
    float vec[8] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
    auto openPos = line.find('[');
    auto closePos = line.find(']', openPos + 1);
    if(openPos == std::string::npos || closePos == std::string::npos)
    {
        return false;
    }
    auto cpos = openPos + 1;
    auto comp = 0;
    std::string word;
    while(cpos < closePos)
    {
        word.clear();
        while(line[cpos] != ',' && cpos < closePos)
        {
            if(line[cpos] != ' ')
            {
                word += line[cpos];
            }
            cpos++;
        }
        vec[comp] = std::stof(word);
        comp++;
        cpos++;
    }
    destVec.load(vec);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

modelCommand hashModelCommand(const std::string& command)
{
    if(command == "start_model")
    {
        return MODEL_START;
    }
    else if(command == "start_surface")
    {
        return MODEL_LOAD_SURFACE;
    }
    else if(command == "end_model")
    {
        return MODEL_END;
    }
    else if(command == "//")
    {
        return MODEL_COMMENT;
    }
    return MODEL_INVALID;
}

modelSurfaceParam hashModelSurfaceParam(const std::string& param)
{
    std::vector<std::string> paramList = {"vtx", "absorption", "end_surface"};
    std::size_t found;
    for(auto i = 0; i < paramList.size(); ++i)
    {
        found = param.find(paramList[i]);
        if(found == 0)
        {
            switch(i)
            {
                case 0:
                    return MODEL_SURFACE_VTX;
                case 1:
                    return MODEL_SURFACE_ABSORB;
                case 2:
                    return MODEL_SURFACE_END;
            }
        }
    }
    return MODEL_SURFACE_INVALID;
}

bool loadModel(Model& destModel, const std::string& fileName)
{
    std::ifstream file;
    std::string line;
    file.open(fileName);
    if(!file.good())
    {
        return false;
    }
    
    bool awaitingCommand = true;
    while(std::getline(file,line))
    {
        if(line == "start_surface;" && awaitingCommand)
        {
            awaitingCommand = false;
            if(loadSurface(destModel, file))
            {
                awaitingCommand = true;
            }
            else
            {
                file.close();
                return false;
            }
        }
    }
    file.close();
    return true;
}

bool loadSurface(Model& destModel, std::ifstream& file)
{
    std::string line;
    std::string word;
    Polygon poly;
    
    // Get vertices
    while(std::getline(file, line))
    {
        if(line == "end_surface;")
        {
            std::vector<Triangle> triangles = triangulatePolygon(poly);
            destModel.appendSurfaces(triangles);
            return true;
        }
        auto vtxPos = line.find("vtx");
        auto absorptionPos = line.find("absorption");
        auto materialPos = line.find("material");
        if(vtxPos != std::string::npos)
        {
            float vtx[3] = {0.f, 0.f, 0.f};
            Vec3f newVec;
            auto openBracketPos = line.find("[");
            auto closeBracketPos = line.find("]", openBracketPos + 1);
            if(openBracketPos == std::string::npos || closeBracketPos == std::string::npos)
            {
                return false;
            }
            
            auto cpos = openBracketPos + 1;
            auto comp = 0;
            while(cpos < closeBracketPos)
            {
                word.clear();
                while(line[cpos] != ',' && cpos < closeBracketPos)
                {
                    if(line[cpos] != ' ')
                    {
                        word += line[cpos];
                    }
                    cpos++;
                }
                vtx[comp] = std::stof(word);
                comp++;
                cpos++;
            }
            newVec.load(vtx);
            poly.addVertex(newVec);
        }
        else if(absorptionPos != std::string::npos)
        {
            float absorption[8] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
            Vec8f newVec;
            auto openBracketPos = line.find("[");
            auto closeBracketPos = line.find("]", openBracketPos + 1);
            if(openBracketPos == std::string::npos || closeBracketPos == std::string::npos)
            {
                return false;
            }
            
            auto cpos = openBracketPos + 1;
            auto comp = 0;
            while(cpos < closeBracketPos)
            {
                word.clear();
                while(line[cpos] != ',' && cpos < closeBracketPos)
                {
                    if(line[cpos] != ' ')
                    {
                        word += line[cpos];
                    }
                    cpos++;
                }
                absorption[comp] = std::stof(word);
                comp++;
                cpos++;
            }
            newVec.load(absorption);
            poly.setAbsorbCoeffs(newVec);
        }
        else if(materialPos != std::string::npos)
        {
            std::cout << "found material" << std::endl;
        }
    }
    return true;
}

// New and improved function

bool loadModel2(Model& destModel, const std::string& fileName)
{
    std::ifstream file;
    std::string line;
    std::vector<std::string> commandList = {"start_model", "start_surface", "//"};
    std::size_t found;
    bool commandMatch = false;
    uint64_t lineCounter = 0;
    file.open(fileName);
    if(!file.good())
    {
        return false;
    }
    bool error = true;
    
    while(std::getline(file,line))
    {
        lineCounter++;
        for(auto i = 0; i < commandList.size(); ++i)
        {
            commandMatch = false;
            found = line.find(commandList[i]);
            if(found == 0)
            switch(i)
            {
                case MODEL_START:
                    commandMatch = true;
                    error = false;
                    break;
                case MODEL_LOAD_SURFACE:
                    error = !loadSurface2(destModel, file, lineCounter);
                    commandMatch = true;
                    break;
                case MODEL_COMMENT:
                    commandMatch = true;
                    error = false;
                    break;
            }
            if(commandMatch)
            {
                break;
            }
        }
        if(line.size() == 0)
        {
            continue;
        }
        if(error)
        {
            file.close();
            printf("Error on line %d\n", lineCounter);
            return false;
        }
    }
    file.close();
    return true;
}

bool loadSurface2(Model& destModel, std::ifstream& file, uint64_t& lineCounter)
{
    std::string line;
    std::vector<std::string> params = {"vtx", "absorption", "diffusion", "scattering", "end_surface", "//"};
    std::size_t found;
    bool paramMatched = false;
    auto paramsFound = 0;
    bool endSurface = false;
    std::vector<Triangle> surfaces;
    Polygon newPolygon;
    Vec3f vertex(0.0, 0.0, 0.0);
    Vec8f absorption(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    double diffusion = 0.f;
    double scattering = 0.f;
    
    while(std::getline(file, line))
    {
        lineCounter++;
        for(auto i = 0; i < params.size(); ++i)
        {
            paramMatched = false;
            found = line.find(params[i]);
            if(found == 0)
            {
                switch(i)
                {
                    case MODEL_SURFACE_VTX:
                        if(!loadVec3f(line, vertex))
                        {
                            return false;
                        }
                        newPolygon.addVertex(vertex);
                        paramMatched = true;
                        break;
                    case MODEL_SURFACE_ABSORB:
                        if(!loadVec8f(line, absorption))
                        {
                            return false;
                        }
                        ++paramsFound;
                        paramMatched = true;
                        break;
                    case MODEL_SURFACE_DIFFUSION:
                        diffusion = loadFloat(line);
                        ++paramsFound;
                        paramMatched = true;
                        break;
                    case MODEL_SURFACE_SCATTERING:
                        scattering = loadFloat(line);
                        ++paramsFound;
                        paramMatched = true;
                        break;
                    case MODEL_SURFACE_END:
                        endSurface = true;
                        paramMatched = true;
                        break;
                    case MODEL_SURFACE_COMMENT:
                        paramMatched = true;
                        break;
                }
                if(paramMatched)
                {
                    break;
                }
            }
        }
        if(endSurface)
        {
            break;
        }
    }
    if(endSurface)
    {
        newPolygon.setAbsorbCoeffs(absorption);
        newPolygon._diffusion = diffusion;
        newPolygon._scattering = scattering;
        surfaces = triangulatePolygon(newPolygon);
        destModel.appendSurfaces(surfaces);
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

configLoaderCommand hashConfigCommand(const std::string& command)
{
    if(command == "start_config;")
    {
        return CONFIG_START;
    }
    if(command == "start_source;")
    {
        return CONFIG_LOAD_SOURCE;
    }
    if(command == "start_receiver;")
    {
        return CONFIG_LOAD_RECEIVER;
    }
    if(command == "end_config;")
    {
        return CONFIG_END;
    }
    return CONFIG_INVALID;
}

bool loadConfig(Model& destModel, const std::string& fileName)
{
    std::ifstream file;
    std::string line;
    file.open(fileName);
    if(!file.good())
    {
        return false;
    }
    bool error = false;
    while(std::getline(file,line))
    {
        switch(hashConfigCommand(line))
        {
            case CONFIG_START:
                break;
            case CONFIG_LOAD_SOURCE:
                error = !loadSource(destModel, file);
                break;
            case CONFIG_LOAD_RECEIVER:
                error = !loadReceiver(destModel, file);
                break;
            case CONFIG_END:
                break;
            case CONFIG_INVALID:
                error = true;
        }
        if(error)
        {
            file.close();
            return false;
        }
    }
    file.close();
    return true;
}

bool loadSource(Model& destModel, std::ifstream& file)
{
    std::string line;
    std::vector<std::string> params = {"origin", "normal", "dir", "end_source"};
    std::size_t found;
    bool paramMatched = false;
    auto paramsFound = 0;
    bool endSource = false;
    Source newSource;
    
    while(std::getline(file, line))
    {
        for(auto i = 0; i < params.size(); ++i)
        {
            paramMatched = false;
            found = line.find(params[i]);
            if(found == 0)
            {
                switch(i)
                {
                    case LOAD_SOURCE_ORIGIN:
                        if(!loadVec3f(line, newSource._origin))
                        {
                            return false;
                        }
                        ++paramsFound;
                        paramMatched = true;
                        break;
                    case LOAD_SOURCE_NORMAL:
                        if(!loadVec3f(line, newSource._normal))
                        {
                            return false;
                        }
                        ++paramsFound;
                        paramMatched = true;
                        break;
                    case LOAD_SOURCE_DIR:
                        if(!loadVec8f(line, newSource._directivity))
                        {
                            return false;
                        }
                        ++paramsFound;
                        paramMatched = true;
                        break;
                    case LOAD_SOURCE_END:
                        endSource = true;
                }
                if(paramMatched)
                {
                    break;
                }
            }
        }
        if(endSource)
        {
            break;
        }
    }
    if(endSource && paramsFound == PARAMS_SOURCE)
    {
        destModel._source = newSource;
        return true;
    }
    return false;
}

loadReceiverParams hashLoadReceiverParam(const std::string& param)
{
    auto pos = param.find("channel");
    if(pos == 0)
    {
        return LOAD_RECEIVER_CHANNEL;
    }
    pos = param.find("origin");
    if(pos == 0)
    {
        return LOAD_RECEIVER_ORIGIN;
    }
    pos = param.find("normal");
    if(pos == 0)
    {
        return LOAD_RECEIVER_NORMAL;
    }
    pos = param.find("pattern");
    if(pos == 0)
    {
        return LOAD_RECEIVER_PATTERN;
    }
    pos = param.find("radius");
    if(pos == 0)
    {
        return LOAD_RECEIVER_RADIUS;
    }
    pos = param.find("end_receiver");
    if(pos == 0)
    {
        return LOAD_RECEIVER_END;
    }
    return LOAD_RECEIVER_INVALID;
}

float hashPolarPattern(const std::string& line)
{
    auto found = line.find("OMNI");
    if(found != std::string::npos)
    {
        return POLAR_PATTERN_OMNI;
    }
    found = line.find("SUB_CARDIOID");
    if(found != std::string::npos)
    {
        return POLAR_PATTERN_SUB_CARDIOID;
    }
    found = line.find("CARDIOID");
    if(found != std::string::npos)
    {
        return POLAR_PATTERN_CARDIOID;
    }
    found = line.find("SUPER_CARDIOD");
    if(found != std::string::npos)
    {
        return POLAR_PATTERN_SUPER_CARDIOID;
    }
    found = line.find("HYPER_CARDIOID");
    if(found != std::string::npos)
    {
        return POLAR_PATTERN_HYPER_CARDIOID;
    }
    found = line.find("FIG_8");
    if(found != std::string::npos)
    {
        return POLAR_PATTERN_FIGURE_OF_EIGHT;
    }
    return POLAR_PATTERN_OMNI;
}

bool loadReceiver(Model& destModel, std::ifstream& file)
{
    std::string line;
    Receiver newReceiver;
    bool end = false;
    auto paramsFound = 0;
    
    while(std::getline(file, line))
    {
        switch(hashLoadReceiverParam(line))
        {
            case LOAD_RECEIVER_CHANNEL:
                newReceiver._channel = loadFloat(line);
                ++paramsFound;
                break;
            case LOAD_RECEIVER_ORIGIN:
                if(!loadVec3f(line, newReceiver._origin))
                {
                    return false;
                }
                ++paramsFound;
                break;
            case LOAD_RECEIVER_NORMAL:
                if(!loadVec3f(line, newReceiver._normal))
                {
                    return false;
                }
                ++paramsFound;
                break;
            case LOAD_RECEIVER_PATTERN:
                newReceiver._polarPattern = hashPolarPattern(line);
                ++paramsFound;
                break;
            case LOAD_RECEIVER_RADIUS:
                newReceiver._radius = loadFloat(line);
                ++paramsFound;
                break;
            case LOAD_RECEIVER_END:
                end = true;
                break;
            case LOAD_RECEIVER_INVALID:
                return false;
        }
        if(end && paramsFound)
        {
            break;
        }
    }
    if(paramsFound == PARAMS_RECEIVER)
    {
        destModel.addReceiver(newReceiver);
        return true;
    }
    return false;
}
