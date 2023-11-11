#ifndef pointLight_h
#define pointLight_h

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"

class PointLight {
public:
    glm::vec3 position;

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;

    float Kc;
    float Kl;
    float Kq;
    int lightNumber;
    
    PointLight(glm::vec3 pos = glm::vec3(6.0f, 2.98f, -2.7f),  glm::vec4 diff= glm::vec4(0.990, 0.523, -0.00990, 1.0f))
    {
        position = pos;
        ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
        diffuse = diff;
        specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        Kc = 1.0;
        Kl = 0.09;
        Kq = 0.032;
        lightNumber = 1;
    }

    void setUpLight(Shader& lightingShader)
    {
        lightingShader.use();
        lightingShader.setVec4("pointLight.ambient", ambient * ambientOn * isOn);
        lightingShader.setVec4("pointLight.diffuse", diffuse * diffuseOn * isOn);
        lightingShader.setVec4("pointLight.specular", specular * specularOn * isOn);
        lightingShader.setVec3("pointLight.position", position);
        lightingShader.setFloat("pointLight.Kc", Kc);
        lightingShader.setFloat("pointLight.Kl", Kl);
        lightingShader.setFloat("pointLight.Kq", Kq);
    }

    void turnOff()
    {
        isOn = 0.0;
    }
    void turnOn()
    {
        isOn = 1.0;
    }
    void turnAmbientOn()
    {
        ambientOn = 1.0;
    }
    void turnAmbientOff()
    {
        ambientOn = 0.0;
    }
    void turnDiffuseOn()
    {
        diffuseOn = 1.0;
    }
    void turnDiffuseOff()
    {
        diffuseOn = 0.0;
    }
    void turnSpecularOn()
    {
        specularOn = 1.0;
    }
    void turnSpecularOff()
    {
        specularOn = 0.0;
    }

private:
    float ambientOn = 1.0;
    float diffuseOn = 1.0;
    float specularOn = 1.0;
    float isOn = 1.0;
};

#endif /* pointLight_h */