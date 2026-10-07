#include <cstddef>
#include <algorithm>
#include <concepts>
#include <iostream>
#include <limits>
#include <vector>
#include <glad/glad.h>
#include "stb_image.h"
#include <assimp/material.h>
#include <assimp/texture.h>

template<typename T>
concept ValidType = std::same_as<T, unsigned char> ||
                    std::same_as<T, aiTexel>;

template<ValidType T>
unsigned int inline TextureFromData(const T* Data, GLenum InternalFormat, GLenum DataFormat, int Width, int Height)
{
    unsigned int TextureID = 0;
    glGenTextures(1, &TextureID);

    glBindTexture(GL_TEXTURE_2D, TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, DataFormat, GL_UNSIGNED_BYTE, Data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return TextureID;
};

unsigned int inline TextureFromEmbedded(const aiTexture& EmbeddedTexture, bool bFlipVertically)
{
    if (EmbeddedTexture.mHeight == 0)
    {
        if (EmbeddedTexture.mWidth == 0 ||
            EmbeddedTexture.mWidth > static_cast<unsigned int>(std::numeric_limits<int>::max()))
        {
            std::cerr << "Embedded texture has an invalid compressed size." << std::endl;
            return 0;
        }

        stbi_set_flip_vertically_on_load(bFlipVertically);

        int Width = 0;
        int Height = 0;
        int NrComponents = 0;
        stbi_uc* Data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(EmbeddedTexture.pcData),
                                              static_cast<int>(EmbeddedTexture.mWidth), &Width, &Height, &NrComponents,
                                              STBI_rgb_alpha);
        if (!Data)
        {
            std::cerr << "Failed to decode embedded texture: " << stbi_failure_reason() << std::endl;
            return 0;
        }

        const unsigned int TextureID = TextureFromData(Data, GL_RGBA8, GL_RGBA, Width, Height);
        stbi_image_free(Data);
        return TextureID;
    }

    if (EmbeddedTexture.mWidth == 0 || EmbeddedTexture.mHeight == 0 ||
        EmbeddedTexture.mWidth > static_cast<unsigned int>(std::numeric_limits<int>::max()) ||
        EmbeddedTexture.mHeight > static_cast<unsigned int>(std::numeric_limits<int>::max()))
    {
        std::cerr << "Embedded texture has invalid dimensions." << std::endl;
        return 0;
    }

    const int Width = static_cast<int>(EmbeddedTexture.mWidth);
    const int Height = static_cast<int>(EmbeddedTexture.mHeight);
    if (!bFlipVertically)
    {
        return TextureFromData(EmbeddedTexture.pcData, GL_RGBA8, GL_BGRA, Width, Height);
    }

    const std::size_t PixelCount = static_cast<std::size_t>(Width) * static_cast<std::size_t>(Height);
    std::vector<aiTexel> FlippedPixels(PixelCount);
    for (int Row = 0; Row < Height; ++Row)
    {
        const std::size_t SourceOffset = static_cast<std::size_t>(Row) * Width;
        const std::size_t DestinationOffset = static_cast<std::size_t>(Height - 1 - Row) * Width;
        std::copy_n(EmbeddedTexture.pcData + SourceOffset, Width, FlippedPixels.data() + DestinationOffset);
    }

    return TextureFromData(FlippedPixels.data(), GL_RGBA8, GL_BGRA, Width, Height);
}

unsigned int inline TextureFromFile(std::string fileName, std::string& directory, bool bFlipImageVerticallyOnLoad = false)
{
    stbi_set_flip_vertically_on_load(bFlipImageVerticallyOnLoad);

    int Width = 0;
    int Height = 0;
    int NrComponents = 0;
    std::string Path = directory + '/' + fileName;
    stbi_uc* Data = stbi_load(Path.c_str(), &Width, &Height, &NrComponents, STBI_rgb_alpha);

    if (Data)
    {
        const unsigned int TextureID = TextureFromData(Data, GL_RGBA8, GL_RGBA, Width, Height);
        stbi_image_free(Data);
        return TextureID;
    }

    std::cerr << "PFTexture failed to load at path: " << Path;
    if (NrComponents == 0 && stbi_failure_reason())
        std::cerr << " (" << stbi_failure_reason() << ')';
    std::cerr << std::endl;
    return 0;
};
