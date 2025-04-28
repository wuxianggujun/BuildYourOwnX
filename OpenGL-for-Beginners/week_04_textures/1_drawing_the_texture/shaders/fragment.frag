#version 330 core

in vec3 fragmentColor;
in vec2 fragmentTexCoord;

out vec4 screenColor;

uniform sampler2D material;

void main(){
    screenColor = texture(material, fragmentTexCoord);
}