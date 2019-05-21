//
//  CppWav.cpp
//  libsndTest
//
//  Created by Dale Johnson on 20/10/2015.
//  Copyright © 2015 Dale Johnson. All rights reserved.
//

#include "CppWav.h"

int BitDepthToSF_Format(CppWavBitDepth bitDepth)
{
    int format = SF_FORMAT_WAV;
    switch(bitDepth)
    {
        case WAV_16_BIT:
            format = format | SF_FORMAT_PCM_16;
            break;
        case WAV_24_BIT:
            format = format | SF_FORMAT_PCM_24;
            break;
        case WAV_32_BIT:
            format = format | SF_FORMAT_PCM_32;
            break;
        default:
            format = format | SF_FORMAT_PCM_16;
    }
    return format;
}

CppWav::CppWav(int64_t frames, int channels, int sRate,
               CppWavBitDepth bitDepth)
{
    info.frames = frames;
    info.samplerate = sRate;
    info.channels = channels;
    info.format = BitDepthToSF_Format(bitDepth);
    _buffer.assign(info.frames * info.channels, 0.0);
}

CppWav::CppWav(const std::string& fileName)
{
    SndfileHandle inFile(fileName);
    info.frames = inFile.frames();
    info.samplerate = inFile.samplerate();
    info.channels = inFile.channels();
    info.format = inFile.format();
    _buffer.resize(info.frames * info.channels);
    inFile.readf(_buffer.data(), info.frames);
}

int64_t CppWav::getNumFrames() const
{
    return info.frames;
}

int CppWav::getNumChannels() const
{
    return info.channels;
}

int CppWav::getSampleRate() const
{
    return info.samplerate;
}

double CppWav::getSample(int64_t frame, int channel)
{
    return _buffer[(frame * info.channels) + channel];
}

void CppWav::setSample(int64_t frame, int channel, double data)
{
    if(data > 1.0)
    {
        _buffer[(frame * info.channels) + channel] = 1.0;
    }
    else if(data < -1.0)
    {
        _buffer[(frame * info.channels) + channel] = -1.0;
    }
    else
    {
        _buffer[(frame * info.channels) + channel] = data;
    }
}

void CppWav::saveFile(const std::string& fileName)
{
    SndfileHandle outFile(fileName, SFM_WRITE, info.format, info.channels, info.samplerate);
    if(!outFile)
    {
        return;
    }
    outFile.writef(_buffer.data(), info.frames);
}