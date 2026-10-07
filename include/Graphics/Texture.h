#pragma once

#include <string>

struct PFTexture
{
    unsigned int mID;
    std::string mType;
};

struct PFTextureCreationInfo
{
    unsigned int mIndex = 0;
    unsigned int mId = 0;
    std::string mPath;
    std::string mType;
    const class aiTexture* embeddedTexture = nullptr;
    bool bFlipTextureOnLoad = false;
};