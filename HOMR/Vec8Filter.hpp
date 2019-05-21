//
//  octaveBandFilter.hpp
//  octaveBandFilter
//
//  Created by Dale Johnson on 10/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#ifndef EIGHT_BAND_VECTOR_FILTER_HPP
#define EIGHT_BAND_VECTOR_FILTER_HPP
#define USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include <vector>
#define VECTORMATH 0
#include "./vectorclass/vectorclass.h"
#include "./vectorclass/vectormath_lib.h"

struct Vec8FilterBase
{
    Vec8FilterBase(unsigned int order, float sampleRate);
    void process();
    void blockForward(std::vector<Vec8f>& data);
    void blockBackward(std::vector<Vec8f>& data);
    void clear();
    virtual void setCutoffFrequencies(const Vec8f& freqs) = 0;
    virtual void calculateCoeffs() = 0;
    unsigned int _order;
    unsigned int _n;
    float _sampleRate;
    Vec8f _in, _out;
    std::vector<Vec8f> _a1, _a2;
    std::vector<Vec8f> _b0, _b1, _b2;
    Vec8f _freqs;
    Vec8f _freqs2;
    Vec8f _freqs3;
    std::vector<Vec8f> _x0, _x1, _x2;
    std::vector<Vec8f> _y0, _y1, _y2;
};

////////////////////////////////////////////////////////////////////////////////

class Vec8LowpassFilters : public Vec8FilterBase
{
public:
    Vec8LowpassFilters(unsigned int order, float sampleRate);
    Vec8LowpassFilters(const Vec8f& freq, unsigned int order, float sampleRate);
    void setCutoffFrequencies(const Vec8f& freqs);
private:
    void calculateCoeffs();
};

////////////////////////////////////////////////////////////////////////////////

class Vec8HighpassFilters : public Vec8FilterBase
{
public:
    Vec8HighpassFilters(unsigned int order, float sampleRate);
    Vec8HighpassFilters(const Vec8f& freq, unsigned int order, float sampleRate);
    void setCutoffFrequencies(const Vec8f& freqs);
private:
    void calculateCoeffs();
};

////////////////////////////////////////////////////////////////////////////////

class Vec8LinearPhaseBandpass
{
public:
    Vec8LinearPhaseBandpass(const Vec8f& lowfreqs, const Vec8f& highFreqs,
                            unsigned int order, float sampleRate);
    void blockProcess(std::vector<Vec8f>& data);
    void setInput(const Vec8f& input);
    void setLowFrequency(unsigned int band, float frequency);
    void setHighFrequency(unsigned int band, float frequency);
    Vec8f getOutput() const;
    void clear();
private:
    std::unique_ptr<Vec8LowpassFilters> _lowpass;
    std::unique_ptr<Vec8HighpassFilters> _highpass;
};

#endif /* octaveBandFilter_hpp */
