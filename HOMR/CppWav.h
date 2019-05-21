//
//  CppWav.h
//
//  Created by Dale Johnson on 20/10/2015.
//  Copyright © 2015 Dale Johnson. All rights reserved.
//

#ifndef _DSJ_CLASS_CPPWAV_
#define _DSJ_CLASS_CPPWAV_
#include <sndfile.hh>
#include <cstdint>
#include <vector>

typedef enum
{
    WAV_16_BIT = 0x0,
    WAV_24_BIT,
    WAV_32_BIT
}CppWavBitDepth;

int BitDepthToSF_Format(CppWavBitDepth bitDepth);
int SF_FormatToBitDepth(int SF_Format);

class CppWav
{
public:
    CppWav(int64_t numSamples, int channels, int sRate,
           CppWavBitDepth bitDepth);
    CppWav(const std::string& fileName);
    int64_t getNumFrames() const;
    int getNumChannels() const;
    int getSampleRate() const;
    double getSample(int64_t frame, int channel);
    void setSample(int64_t frame, int channel, double data);
    void saveFile(const std::string& fileName);
private:
    SF_INFO info;
    std::vector<double> _buffer;
};


#endif /* CppLibSnd_h */
