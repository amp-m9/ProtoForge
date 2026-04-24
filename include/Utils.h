#include <cstddef>
template <typename T, size_t N> // returns size of C-style arrays
unsigned int Length(T (&Array)[N])
{
    if (sizeof(Array))
    {
        return sizeof(Array) / sizeof(*Array);
    }

    return 0;
}

unsigned int TextureFromFile(std::string fileName, std::string& directory, bool gamma = false)
{
    std::string Path = directory + '/' + fileName;

    unsigned int TextureID;
    glGenTextures(1, &TextureID);

    int Width, Height, NrComponents;
    unsigned char *Data = stbi_load(Path.c_str(), &Width, &Height, &NrComponents, 0);
    
    if(Data)
    {
        GLenum Format;

        if (NrComponents == 1)
            Format = GL_RED;
        else if (NrComponents == 3)
            Format = GL_RGB;
        else if (NrComponents == 4)
            Format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }
    else
    {
        std::cout << "Texture failed to load at path: " << Path << std::endl;
    }
    
    stbi_image_free(Data);
    return TextureID;
}