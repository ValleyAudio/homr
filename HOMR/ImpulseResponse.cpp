//
//  ImpulseResponse.cpp
//  HOMR2
//
//  Created by Dale Johnson on 18/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#include "ImpulseResponse.hpp"

RawResponse::RawResponse(uint64_t channels)
{
    _response.resize(channels);
    _maxDistance = 0.f;
    _maxTime = 0.f;
}

void RawResponse::append(const uint64_t channel, const MetaRay &ray)
{
    MetaRay newRay(ray);
    newRay._normal *= -1.0;
    _response[channel].push_back(newRay);
    if(ray._distance > _maxDistance)
    {
        _maxDistance = ray._distance;
        _maxTime = _maxDistance * TIME_OF_SOUND;
    }
}

uint64_t RawResponse::getNumChannels() const
{
    return _response.size();
}

float RawResponse::getMaxDistance() const
{
    return _maxDistance;
}

float RawResponse::getMaxTime() const
{
    return _maxTime;
}

const std::vector<MetaRay>& RawResponse::getChannelData(uint64_t channel) const
{
    return _response[channel];
}

const std::vector<std::vector<MetaRay>>& RawResponse::getConstData() const
{
    return _response;
}

std::vector<std::vector<MetaRay>>& RawResponse::getData()
{
    return _response;
}

void normaliseRawResponse(RawResponse& response)
{
    float maxValue = 0.f;
    for(auto i = 0; i < response.getData().size(); i++)
    {
        std::vector<MetaRay> channelData = response.getData()[i];
        for(auto j = 0; j < channelData.size(); j++)
        {
            for(auto k = 0; k < 8; k++)
            {
                if(channelData[j]._energy[k] > maxValue)
                {
                    maxValue = channelData[j]._energy[k];
                }
            }
        }
    }
    maxValue = 1.f / maxValue;
    for(auto i = 0; i < response.getData().size(); i++)
    {
        for(auto j = 0; j < response.getData()[i].size(); j++)
        {
            response.getData()[i][j]._energy *= maxValue;
        }
    }
}

void exportRawResponse(const RawResponse& response, const std::string& fileName)
{
    std::ofstream file;
    for(auto i = 0; i < response.getNumChannels(); ++i)
    {
        std::string channelFileName = fileName + "_" + std::to_string(i) + ".csv";
        file.open(channelFileName);
        file.clear();
        if(!file.good())
        {
            return;
        }
        const std::vector<MetaRay>& channelData = response.getChannelData(i);
        for(auto j = 0; j < response.getChannelData(i).size(); ++j)
        {
            // Direction
            file << channelData[j]._normal[0] << ",";
            file << channelData[j]._normal[1] << ",";
            file << channelData[j]._normal[2] << ",";
            
            // Energy
            file << channelData[j]._energy[0] << ",";
            file << channelData[j]._energy[1] << ",";
            file << channelData[j]._energy[2] << ",";
            file << channelData[j]._energy[3] << ",";
            file << channelData[j]._energy[4] << ",";
            file << channelData[j]._energy[5] << ",";
            file << channelData[j]._energy[6] << ",";
            file << channelData[j]._energy[7] << ",";
            
            // Distance
            file << channelData[j]._distance << ",";
            
            // Order
            file << channelData[j]._order << std::endl;
        }
        file.close();
    }
}

////////////////////////////////////////////////////////////////////////////////

ImpulseResponse::ImpulseResponse(uint64_t channels, uint64_t length)
{
    _channels = channels;
    _response.resize(_channels);
    _length = length;
    for(auto i = 0; i < _response.size(); i++)
    {
        _response[i].assign(_length, 0.f);
    }
}

void ImpulseResponse::setSample(uint64_t channel, uint64_t sample, float data)
{
    _response[channel][sample] = data;
}

uint64_t ImpulseResponse::getNumChannels() const
{
    return _channels;
}

uint64_t ImpulseResponse::getLength() const
{
    return _length;
}

float ImpulseResponse::getSample(uint64_t channel, uint64_t sample) const
{
    return _response[channel][sample];
}

const std::vector<float>& ImpulseResponse::getChannelData(uint64_t channel)
{
    return _response[channel];
}

ExtractionPlan::ExtractionPlan()
{
    _all = true;
    _allSurfaces = true;
    _allOrders = true;
    _allTimeRange = true;
    _startTime = 0.f;
    _endTime = 0.f;
}

ExtractionPlan extractEntireRaw()
{
    return ExtractionPlan();
}

ExtractionPlan extractFromSurface(const std::vector<int>& surfaces)
{
    ExtractionPlan plan;
    plan._all = false;
    plan._allSurfaces = false;
    plan._surfaces = surfaces;
    return plan;
}

ExtractionPlan extractTimeRange(float startTime, float endTime)
{
    ExtractionPlan plan;
    plan._all = false;
    plan._allTimeRange = false;
    plan._startTime = startTime;
    plan._endTime = endTime;
    return plan;
}

ExtractionPlan extractOrderRange(uint32_t lowOrder, uint32_t highOrder)
{
    ExtractionPlan plan;
    plan._all = false;
    plan._allOrders = false;
    for(auto i = lowOrder; i <= highOrder; i++)
    {
        plan._orders.push_back(i);
    }
    return plan;
}

ExtractionPlan extractOrders(const std::vector<uint32_t>& orders)
{
    ExtractionPlan plan;
    plan._all = false;
    plan._allOrders = false;
    plan._orders = orders;
    return plan;
}

bool rayFromTargetSurfaces(const MetaRay& ray, const std::vector<int>& surfaceList)
{
    for(auto i = 0; i < surfaceList.size(); i++)
    {
        if(ray._surface == surfaceList[i])
        {
            return true;
        }
    }
    return false;
}

bool rayMatchesOrderList(const MetaRay& ray, const std::vector<uint32_t>& orderList)
{
    for(auto i = 0; i < orderList.size(); i++)
    {
        if(ray._order == orderList[i])
        {
            return true;
        }
    }
    return false;
}

ImpulseResponse convertRawToIR(const RawResponse& rawIn, const Model& model,
                               float sampleRate, ExtractionPlan plan)
{
    uint64_t samples = (rawIn.getMaxTime() + 0.02f) * sampleRate;
    ImpulseResponse ir(rawIn.getNumChannels(), samples);
    
    uint64_t samplePos = 0;
    Vec8f energy = 0.f;
    for(auto i = 0; i < rawIn.getNumChannels(); i++)
    {
        //const std::vector<MetaRay>& rays = rawIn.getChannelData(i);
        std::vector<Vec8f> octBandResponse;
        Vec8LinearPhaseBandpass filter(lowFreqs, highFreqs, 8, sampleRate);
        octBandResponse.assign(samples, Vec8f(0.f));
        for(auto j = 0; j < rawIn.getChannelData(i).size(); j++)
        {
            MetaRay ray = rawIn.getChannelData(i)[j];
            if(!plan._all)
            {
                if(!plan._allSurfaces)
                {
                    if(!rayFromTargetSurfaces(ray, plan._surfaces))
                    {
                        continue;
                    }
                }
                if(!plan._allOrders)
                {
                    if(!rayMatchesOrderList(ray, plan._orders))
                    {
                        continue;
                    }
                }
            }
            samplePos = ray._distance * TIME_OF_SOUND * sampleRate;
            energy = ray._energy * calcPolarMagnitude(ray._normal,
                                                      model._receivers[i]._normal,
                                                      model._receivers[i]._polarPattern);
            if(ray._diffused)
            {
                bool zeroEnergy = true;
                for(auto i = 0; i < 8; ++i)
                {
                    if(octBandResponse[samplePos][i] != 0.0)
                    {
                        zeroEnergy = false;
                        break;
                    }
                }
                if(zeroEnergy)
                {
                    octBandResponse[samplePos] = energy;
                }
            }
            else
            {
                octBandResponse[samplePos] = energy;
            }
        }
        filter.blockProcess(octBandResponse);
        for(auto j = 0; j < octBandResponse.size(); j++)
        {
            for(auto k = 0; k < 8; k++)
            {
                ir.setSample(i, j, ir.getSample(i, j) + octBandResponse[j][k]);
            }
        }
        filter.clear();
    }
    return ir;
}

CppWav convertIRToWav(const ImpulseResponse& ir, float sampleRate, CppWavBitDepth bitDepth)
{
    CppWav wav(ir.getLength(), ir.getNumChannels(), sampleRate, bitDepth);
    for(auto i = 0; i < ir.getLength(); i++)
    {
        for(auto j = 0; j < ir.getNumChannels(); j++)
        {
            wav.setSample(i, j, ir.getSample(j, i));
        }
    }
    return wav;
}
