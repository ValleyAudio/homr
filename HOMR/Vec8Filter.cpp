//
//  octaveBandFilter.cpp
//  octaveBandFilter
//
//  Created by Dale Johnson on 10/04/2016.
//  Copyright © 2016 Dale Johnson. All rights reserved.
//

#include "Vec8Filter.hpp"

Vec8FilterBase::Vec8FilterBase(unsigned int order, float sampleRate)
{
    _order = order;
    _n = _order / 2;
    _sampleRate = sampleRate;
    _x0.resize(_n);
    _x1.resize(_n);
    _x2.resize(_n);
    _y0.resize(_n);
    _y1.resize(_n);
    _y2.resize(_n);
    _a1.resize(_n);
    _a2.resize(_n);
    _b0.resize(_n);
    _b1.resize(_n);
    _b2.resize(_n);
    clear();
}

void Vec8FilterBase::process()
{
    _out = _in;
    for (auto i = 0; i < _n; i++)
    {
        _x0[i] = _out;
        _y0[i] = _x0[i] * _b0[i] + _x1[i] * _b1[i] + _x2[i] * _b2[i]
               - _y1[i] * _a1[i] - _y2[i] * _a2[i];
        _y2[i] = _y1[i];
        _y1[i] = _y0[i];
        _x2[i] = _x1[i];
        _x1[i] = _x0[i];
        _out = _y0[i];
    }
}

void Vec8FilterBase::blockForward(std::vector<Vec8f> &data)
{
    for(auto i = 0; i < data.size(); ++i)
    {
        _in = data[i];
        process();
        data[i] = _out;
    }
}

void Vec8FilterBase::blockBackward(std::vector<Vec8f> &data)
{
    for(long i = data.size() - 1; i >= 0; --i)
    {
        _in = data[i];
        process();
        data[i] = _out;
    }
}

void Vec8FilterBase::clear()
{
    _in = Vec8f(0.f);
    _out = Vec8f(0.f);
    for(auto i = 0; i < _n; i++)
    {
        _x0[i] = Vec8f(0.f);
        _x1[i] = Vec8f(0.f);
        _x2[i] = Vec8f(0.f);
        _y0[i] = Vec8f(0.f);
        _y1[i] = Vec8f(0.f);
        _y2[i] = Vec8f(0.f);
    }
}

////////////////////////////////////////////////////////////////////////////////

Vec8LowpassFilters::Vec8LowpassFilters(unsigned int order, float sampleRate)
                  : Vec8FilterBase(order, sampleRate)
{
    setCutoffFrequencies(Vec8f(1.0));
}

Vec8LowpassFilters::Vec8LowpassFilters(const Vec8f& freqs, unsigned int order, float sampleRate)
                  : Vec8FilterBase(order, sampleRate)
{
    setCutoffFrequencies(freqs);
}

void Vec8LowpassFilters::setCutoffFrequencies(const Vec8f& freqs)
{
    _freqs = tan(M_PI * freqs/ _sampleRate);
    _freqs2 = _freqs * _freqs;
    _freqs3 = _freqs2 * _freqs;
    calculateCoeffs();
};

void Vec8LowpassFilters::calculateCoeffs()
{
    Vec8f p;
    Vec8f gain;
    for (auto i = 0; i < _n; i++)
    {
        p = 2.f * _freqs * cos(M_PI * (_order + 2.f * (i + 1) - 1.f) / (2.f * _order));
        gain = 1.f - p + _freqs2;
        _b0[i] = _freqs2 / gain;
        _b1[i] = 2.f * _b0[i];
        _b2[i] = _b0[i];
        _a1[i] = (2.f * (_freqs2 - 1.f)) / gain;
        _a2[i] = (_freqs2 + p + 1.f) / gain;
    }
    if (_order % 2)
    {
        gain = 1.f + _freqs;
        _b0[_order - 1] = _freqs / gain;
        _b1[_order - 1] = _b0[_order - 1];
        _b2[_order - 1] = 0.f;
        _a1[_order - 1] = (_freqs - 1.f) / gain;
        _a2[_order - 1] = 0.f;
    }
}

////////////////////////////////////////////////////////////////////////////////

Vec8HighpassFilters::Vec8HighpassFilters(unsigned int order, float sampleRate)
                   : Vec8FilterBase(order, sampleRate)
{
    setCutoffFrequencies(Vec8f(1.f));
}

Vec8HighpassFilters::Vec8HighpassFilters(const Vec8f& freqs, unsigned int order, float sampleRate)
                   : Vec8FilterBase(order, sampleRate)
{
    setCutoffFrequencies(freqs);
}

void Vec8HighpassFilters::setCutoffFrequencies(const Vec8f& freqs)
{
    _freqs = tan(M_PI * freqs/ _sampleRate);
    _freqs2 = _freqs * _freqs;
    _freqs3 = _freqs2 * _freqs;
    calculateCoeffs();
};

void Vec8HighpassFilters::calculateCoeffs()
{
    Vec8f p;
    Vec8f gain;
    for (auto i = 0; i < _n; i++)
    {
        p = 2.f * _freqs2 * cos(M_PI * (_order + 2.f * (i + 1) - 1.f) / (2.f * _order));
        gain = _freqs - p + _freqs3;
        _b0[i] = _freqs / gain;
        _b1[i] = -2.f * _b0[i];
        _b2[i] = _b0[i];
        _a1[i] = (2.f * (_freqs3 - _freqs)) / gain;
        _a2[i] = (_freqs3 + p + _freqs) / gain;
    }
    if (_order % 2)
    {
        gain = 1.f + _freqs;
        _b0[_order - 1] = 1.f / gain;
        _b1[_order - 1] = -_b0[_order - 1];
        _b2[_order - 1] = 0.f;
        _a1[_order - 1] = (_freqs - 1.f) / gain;
        _a2[_order - 1] = 0.f;
    }
}

////////////////////////////////////////////////////////////////////////////////

Vec8LinearPhaseBandpass::Vec8LinearPhaseBandpass(const Vec8f& lowfreqs, const Vec8f& highFreqs,
                                                 unsigned int order, float sampleRate)
{
    _lowpass = std::unique_ptr<Vec8LowpassFilters>(new Vec8LowpassFilters(order, sampleRate));
    _highpass = std::unique_ptr<Vec8HighpassFilters>(new Vec8HighpassFilters(order, sampleRate));
    _highpass->setCutoffFrequencies(lowfreqs);
    _lowpass->setCutoffFrequencies(highFreqs);
}

void Vec8LinearPhaseBandpass::blockProcess(std::vector<Vec8f>& data)
{
    _lowpass->blockForward(data);
    _lowpass->clear();
    _lowpass->blockBackward(data);
    _highpass->blockForward(data);
    _highpass->clear();
    _highpass->blockBackward(data);
}

void Vec8LinearPhaseBandpass::clear()
{
    _lowpass->clear();
    _highpass->clear();
}
