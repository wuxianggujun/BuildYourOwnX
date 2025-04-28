//
// Created by wuxianggujun on 2025/4/27.
//

#pragma once

#include "Config.h"


class Material {
public:
    Material(const char* fileName);
    ~Material();

    void use();

private:
    unsigned int texture;
    
};


