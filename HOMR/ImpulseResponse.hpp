//
//  ImpulseResponse.hpp
//  HOMR2
//
//  Created by Dale Johnson on 18/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#ifndef HOMR_IMPULSE_RESPONSE
#define HOMR_IMPULSE_RESPONSE

#include "AcousticObjects.hpp"
#include "Vec8Filter.hpp"
#include "Model.hpp"
#include "CppWav.h"
#include <fstream>

static Vec8f lowFreqs = Vec8f(44.f, 88.f, 176.f, 352.f, 704.f, 1408.f, 2816.f, 5632.f);
static Vec8f highFreqs = Vec8f(88.f, 176.f, 352.f, 704.f, 1408.f, 2816.f, 5632.f, 22000.f);

class RawResponse
{
public:
    RawResponse(uint64_t channels);
    void append(const uint64_t channel, const MetaRay& ray);
    uint64_t getNumChannels() const;
    float getMaxDistance() const;
    float getMaxTime() const;
    const std::vector<MetaRay>& getChannelData(uint64_t channel) const;
    const std::vector<std::vector<MetaRay>>& getConstData() const;
    std::vector<std::vector<MetaRay>>& getData();
private:
    float _maxDistance;
    float _maxTime;
    std::vector<std::vector<MetaRay>> _response;
};

void normaliseRawResponse(RawResponse& response);
void exportRawResponse(const RawResponse& response, const std::string& fileName);

////////////////////////////////////////////////////////////////////////////////

class ImpulseResponse
{
public:
    ImpulseResponse(uint64_t channels, uint64_t length);
    void setSample(uint64_t channel, uint64_t sample, float data);
    uint64_t getNumChannels() const;
    uint64_t getLength() const;
    float getSample(uint64_t channel, uint64_t sample) const;
    const std::vector<float>& getChannelData(uint64_t channel);
private:
    uint64_t _channels;
    uint64_t _length;
    std::vector<std::vector<float>> _response;
};

struct ExtractionPlan
{
    ExtractionPlan();
    bool _all;
    bool _allSurfaces;
    bool _allOrders;
    bool _allTimeRange;
    
    std::vector<int> _surfaces;
    std::vector<uint32_t> _orders;
    float _startTime;
    float _endTime;
};

ExtractionPlan extractEntireRaw();
ExtractionPlan extractFromSurfaces(const std::vector<int>& surfaces);
ExtractionPlan extractTimeRange(float startTime, float endTime);
ExtractionPlan extractOrderRange(uint32_t lowOrder, uint32_t highOrder);
ExtractionPlan extractOrderList(const std::vector<uint32_t>& orders);

bool rayFromTargetSurfaces(const MetaRay& ray, const std::vector<int>& surfaceList);
bool rayMatchesOrder(const MetaRay& ray, const std::vector<uint32_t>& orderList);

ImpulseResponse convertRawToIR(const RawResponse& rawIn, const Model& model,
                               float sampleRate, ExtractionPlan plan);

CppWav convertIRToWav(const ImpulseResponse& ir, float sampleRate, CppWavBitDepth bitDepth);

#endif /* ImpulseResponse_hpp */
