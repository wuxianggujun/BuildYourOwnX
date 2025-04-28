//
// Created by wuxianggujun on 2025/4/27.
//

#include "material.h"
#include "stb_image.h"

Material::Material(const char* fileName)
{
    int width, height, channels;
    unsigned char* data = stbi_load(fileName, &width, &height, &channels, STBI_rgb_alpha);

    std::cout << "Loading image: " << fileName << std::endl;
    std::cout << "Width: " << width << " Height: " << height << " Channels: " << channels << std::endl;
    
    // make the texture
    glGenTextures( 1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // upload the texture data
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, width, height, 0,GL_RGBA,GL_UNSIGNED_BYTE, data);

    // free data
    stbi_image_free(data);

    // configure sampler
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    
}

Material::~Material()
{
    glDeleteTextures(1, &texture);
}

void Material::use()
{
    glBindTexture(GL_TEXTURE_2D, texture);
}
