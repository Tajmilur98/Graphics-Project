//
//  main.cpp
//  3D Object Drawing
//


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "directionalLight.h"
#include "pointLight.h"
#include "spotLight.h"

#include "stb_image.h"
#include "dome.h"

#include <iostream>
#include <stdlib.h>
#include<windows.h> 



using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);



void drawCube(Shader ourShader, glm::mat4 moveMatrix);
void drawDoor(Shader ourShader);
void drawMinar(Shader ourShader);
void drawBookShelf(Shader ourShader);
float k = 0.0f;
void drawFloor(Shader ourShader, unsigned int VAO);
void drawGate(Shader ourShader,Shader bulbShader, unsigned int VAO);
void drawWalls(Shader ourShader);
void drawPerson(Shader ourShader,unsigned int VAO);
float person_rotate = 0.0f;


void load_texture(unsigned int& texture, string image_name, GLenum format);

// settings
const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 800;

// modelling transform
float rotateAngle_X = 0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 0.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 1.0;

// camera
Camera camera(glm::vec3(4.0f, 1.0f, -24.7f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 0.0, eyeY = 1.0, eyeZ = 2.5;
float lookAtX = 0.0, lookAtY = 1.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;


// Light initialization
glm::vec3 lightPositions[] = {
        glm::vec3(6.0f, 2.98f, -2.7f),
        glm::vec3(9.0f, 2.98f, -2.7f),
        glm::vec3(-1.5f, 2.0f, -2.75f)
};

glm::vec3 lightDirections[] = {
    glm::vec3(0.0f, 0.50f, -1.0f),
    glm::vec3(0.0f, -0.2f, 0.0f)
};


DirectionalLight directionalLight(lightDirections[0], glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1);
PointLight pointLight(lightPositions[0],glm::vec4(0.990, 0.523, -0.00990, 1.0f));
SpotLight spotLight(lightPositions[1], lightDirections[1], 8.5f, 15.5f, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, 1);


float directionalLightOn = 0.0;
float pointLightOn = 1.0;
float spotLightOn = 0.0;


bool lightingOn = false;
float ambientOn = 1.0;
float diffuseOn = 1.0;
float specularOn = 1.0;
bool dark = false;


// texture
float extraX = 24.0f;
float extraY = 12.0f;
float TXmin = 0.0f;
float TXmax = 1.0f + extraX;
float TYmin = 0.0f;
float TYmax = 1.0f + extraY;


//rotate
float r = 6, velocity = 0;
bool fanOff = true, lampOff = false;


// Textures
unsigned int texture[100];
unsigned int cubeVAO, cubeVAO11, cubeVAO51;


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSE 4208: Computer Graphics Laboratory", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");
    Shader lightCubeShader("lightVertexShader.vs", "lightFragmentShader.fs");
    
    

    float cube_vertices[] = {
        // positions      // normals         // texture coords
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, TXmax, TYmin,
        0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, TXmin, TYmin,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, TXmin, TYmax,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, TXmax, TYmax,

        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, TXmax, TYmin,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, TXmax, TYmax,
        0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, TXmin, TYmin,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, TXmin, TYmax,

        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, TXmin, TYmin,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, TXmax, TYmin,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, TXmax, TYmax,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, TXmin, TYmax,

        0.0f, 0.0f, 0.5f, -1.0f, 0.0f, 0.0f, TXmax, TYmin,
        0.0f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, TXmax, TYmax,
        0.0f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, TXmin, TYmax,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, TXmin, TYmin,

        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, TXmax, TYmin,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, TXmax, TYmax,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, TXmin, TYmax,
        0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, TXmin, TYmin,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, TXmin, TYmin,
        0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, TXmax, TYmin,
        0.5f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f, TXmax, TYmax,
        0.0f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f, TXmin, TYmax
    };
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };
    
    unsigned int  cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // position attribute
   // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)24);
    glEnableVertexAttribArray(2);


    //5x1 VAO

    TXmax = 5;
    TYmax = 1;

    float cube_vertices1[] = {
        // positions      // normals         // texture coords
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, TXmax, TYmin,
        0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, TXmin, TYmin,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, TXmin, TYmax,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, TXmax, TYmax,

        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, TXmax, TYmin,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, TXmax, TYmax,
        0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, TXmin, TYmin,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, TXmin, TYmax,

        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, TXmin, TYmin,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, TXmax, TYmin,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, TXmax, TYmax,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, TXmin, TYmax,

        0.0f, 0.0f, 0.5f, -1.0f, 0.0f, 0.0f, TXmax, TYmin,
        0.0f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, TXmax, TYmax,
        0.0f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, TXmin, TYmax,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, TXmin, TYmin,

        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, TXmax, TYmin,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, TXmax, TYmax,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, TXmin, TYmax,
        0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, TXmin, TYmin,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, TXmin, TYmin,
        0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, TXmax, TYmin,
        0.5f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f, TXmax, TYmax,
        0.0f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f, TXmin, TYmax
    };
    glGenVertexArrays(1, &cubeVAO51);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO51);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices1), cube_vertices1, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // position attribute
   // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)24);
    glEnableVertexAttribArray(2);


    //1x1 VAO
    TXmax = 1;
    TYmax = 1;

    float cube_vertices2[] = {
        // positions      // normals         // texture coords
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, TXmax, TYmin,
        0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, TXmin, TYmin,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, TXmin, TYmax,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, TXmax, TYmax,

        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, TXmax, TYmin,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, TXmax, TYmax,
        0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, TXmin, TYmin,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, TXmin, TYmax,

        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, TXmin, TYmin,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, TXmax, TYmin,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, TXmax, TYmax,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, TXmin, TYmax,

        0.0f, 0.0f, 0.5f, -1.0f, 0.0f, 0.0f, TXmax, TYmin,
        0.0f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, TXmax, TYmax,
        0.0f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, TXmin, TYmax,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, TXmin, TYmin,

        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, TXmax, TYmin,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, TXmax, TYmax,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, TXmin, TYmax,
        0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, TXmin, TYmin,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, TXmin, TYmin,
        0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, TXmax, TYmin,
        0.5f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f, TXmax, TYmax,
        0.0f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f, TXmin, TYmax
    };
    glGenVertexArrays(1, &cubeVAO11);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO11);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices2), cube_vertices2, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // position attribute
   // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)24);
    glEnableVertexAttribArray(2);



    //light's VAO
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);



    //sphere
   

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    

    //ourShader.use();

     // Texture loading

    //Texture 0
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("transparent.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "Failed to load texture 0" << endl;
    }
    stbi_image_free(data);


    load_texture(texture[1], "grass1.jpg", GL_RGB);
    load_texture(texture[2], "floortiles2.jpg", GL_RGB);
    load_texture(texture[3], "leftwall-1.jpg", GL_RGB);
    load_texture(texture[4], "mihrab.jpg", GL_RGB);
    load_texture(texture[5], "frontop.jpg", GL_RGB);
    load_texture(texture[6], "leftwall-1.jpg", GL_RGB);
    load_texture(texture[7], "road.jpg", GL_RGB);
    load_texture(texture[8], "man.png", GL_RGBA);
    load_texture(texture[9], "door.jpg", GL_RGB);
    load_texture(texture[10], "pil.jpg", GL_RGB);

    load_texture(texture[11], "shelftop1.jpg", GL_RGB);
    load_texture(texture[12], "cabinet.jpg", GL_RGB);
    load_texture(texture[13], "cabinetdoor.jpg", GL_RGB);
    load_texture(texture[14], "book1.jpg", GL_RGB);
    load_texture(texture[15], "book3.jpg", GL_RGB);
    

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // activate shader
        ourShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

       

        // camera/view transformation
        //glm::mat4 view = basic_camera.createViewMatrix();
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);


        //**************lighting**************

        //Setting up Directional Light
        directionalLight.setUpLight(ourShader);
        if (!directionalLightOn)
            directionalLight.turnOff();
        if (!ambientOn)
            directionalLight.turnAmbientOff();
        if (!diffuseOn)
            directionalLight.turnDiffuseOff();
        if (!specularOn)
            directionalLight.turnSpecularOff();

        //Setting up Point Light
        
            pointLight.setUpLight(ourShader);
            if (!pointLightOn)
                pointLight.turnOff();
            if (!ambientOn)
                pointLight.turnAmbientOff();
            if (!diffuseOn)
                pointLight.turnDiffuseOff();
            if (!specularOn)
                pointLight.turnSpecularOff();
        


        //Setting up Spot Light
        spotLight.setUpLight(ourShader);
        if (!spotLightOn)
            spotLight.turnOff();
        if (!ambientOn)
            spotLight.turnAmbientOff();
        if (!diffuseOn)
            spotLight.turnDiffuseOff();
        if (!specularOn)
            spotLight.turnSpecularOff();

        //Setting up Camera and Others
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setBool("lightingOn", lightingOn);
        ourShader.setFloat("ambientOn", ambientOn);
        ourShader.setFloat("diffuseOn", diffuseOn);
        ourShader.setFloat("specularOn", specularOn);
        ourShader.setFloat("pointLightOn", pointLightOn);
        ourShader.setFloat("directionalLightOn", directionalLightOn);
        ourShader.setFloat("spotLightOn", spotLightOn);
        ourShader.setBool("dark", dark);

       


        glBindVertexArray(cubeVAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture[1]);
        drawFloor(ourShader, cubeVAO);
        drawWalls(ourShader);
        //drawDoor(ourShader);
        //drawPerson(ourShader,cubeVAO11);       
        //drawGate(ourShader, lightCubeShader, cubeVAO);
        //drawMinar(ourShader);
        drawBookShelf(ourShader);
       

        //Lights
        for (int i = 0; i < 2; i++)
        {
            lightCubeShader.use();

            glm::vec4 bodyColor = glm::vec4(0.970, 0.907, 0.679, 1.0f);

            //emissive

            glm::vec3 val = glm::vec3(0.5f);

            /*if (i == 0 and pointLightOn == 0.0)
                bodyColor = glm::vec4(val, 1.0f);
            if (i == 1 and spotLightOn == 0.0)
                bodyColor = glm::vec4(val, 1.0f);*/
            if (dark)
                bodyColor = glm::vec4(val, 1.0f);
            

            
                lightCubeShader.setVec4("bodyColor", bodyColor);
                glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
                lightCubeShader.setMat4("projection", projection);
                glm::mat4 view = camera.GetViewMatrix();
                lightCubeShader.setMat4("view", view);


                glm::mat4 tempModel = glm::mat4(1.0f);
                tempModel = glm::translate(tempModel, lightPositions[i]);
                tempModel = glm::scale(tempModel, glm::vec3(0.2,0.2,0.2f));
                lightCubeShader.setMat4("model", tempModel);

                glBindVertexArray(lightCubeVAO);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            
        }



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {                 //Forward
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {                 //Backward
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {                 //Left
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {                 //Right
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)                   //Down
    {

        camera.ProcessKeyboard(DOWN, deltaTime);


    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)                   //Pitch positive
    {
      
        camera.ProcessYPR(0.0f, 3.0f, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)                   //Pitch negative
    {
        
        camera.ProcessYPR(0.0f, -3.0f, 0.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)                   //Yaw positive
    {
        camera.ProcessYPR(3.0f, 0.0f, 0.0f);
        person_rotate+=15;

    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)                   //Yaw negative
    {
        
        camera.ProcessYPR(-3.0f, 0.0f, 0.0f);
        person_rotate -= 15;
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)                   //Roll positive
    {
        camera.ProcessYPR(0.0f, 0.0f, 0.5f);

    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)                   //Roll negative
    {
        /*rotateAngle_Z += 0.1;
        rotateAxis_X = 0.0;
        rotateAxis_Y = 0.0;
        rotateAxis_Z = 1.0;*/
        camera.ProcessYPR(0.0f, 0.0f, -0.5f);

    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        eyeX += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)                   //Rotate camera around a look at point
    { 
        camera.RotateAroundLookAt(2.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)                   //Rotate camera around a look at point
    {  
        camera.RotateAroundLookAt(-2.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        eyeZ += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        eyeY += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)                   //Up
    {
        eyeY -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);

        camera.ProcessKeyboard(UP, deltaTime);

    }

    //***************Lighting***************

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)                   //Lighting On-Off
    {
        lightingOn ^= true;
        cout << lightingOn << endl;
        Sleep(100);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)                   //Ambient On
    {
        ambientOn = 1.0;
        pointLight.turnAmbientOn();
        spotLight.turnAmbientOn();
        directionalLight.turnAmbientOn();
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)                   //Ambient Off
    {
        ambientOn = 0.0;
        pointLight.turnAmbientOff();
        spotLight.turnAmbientOff();
        directionalLight.turnAmbientOff();
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)                   //Diffuse On
    {
        diffuseOn = 1.0;
        pointLight.turnDiffuseOn();
        spotLight.turnDiffuseOn();
        directionalLight.turnDiffuseOn();

    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)                   //Diffuse Off
    {
        diffuseOn = 0.0;
        pointLight.turnDiffuseOff();
        spotLight.turnDiffuseOff();
        directionalLight.turnDiffuseOff();
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)                   //Specular On
    {
        specularOn = 1.0;
        pointLight.turnSpecularOn();
        spotLight.turnSpecularOn();
        directionalLight.turnSpecularOn();
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)                   //Specular Off
    {
        specularOn = 0.0;
        pointLight.turnSpecularOff();
        spotLight.turnSpecularOff();
        directionalLight.turnSpecularOff();
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)                   //Point Light On
    {
        pointLightOn = 1.0;
        pointLight.turnOn();
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)                   //Point Light Off
    {
        pointLightOn = 0.0;
        pointLight.turnOff();
    }




    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)                   //Directional Light On
    {
        directionalLightOn = 1.0;
        directionalLight.turnOn();
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)                   //Directional Light Off
    {
        directionalLightOn = 0.0;
        directionalLight.turnOff();
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)                   //Spot Light On
    {
        lampOff = false;
        spotLightOn = 1.0;
        spotLight.turnOn();
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)                   //Spot Light Off
    {
        lampOff = true;
        spotLightOn = 0.0;
        spotLight.turnOff();
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)                   //Dark On-Off
    {
        dark ^= true;
        cout << dark << endl;
        Sleep(100);
    }


    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        lookAtY += 2.5 * deltaTime;
        basic_camera.changeLookAt(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        lookAtY -= 2.5 * deltaTime;
        basic_camera.changeLookAt(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        lookAtZ += 2.5 * deltaTime;
        basic_camera.changeLookAt(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        lookAtZ -= 2.5 * deltaTime;
        basic_camera.changeLookAt(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    {
        basic_camera.changeViewUpVector(glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    {
        basic_camera.changeViewUpVector(glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
    {
        basic_camera.changeViewUpVector(glm::vec3(0.0f, 0.0f, 1.0f));
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


//Texture Loading

void load_texture(unsigned int& texture, string image_name, GLenum format)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(image_name.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "Failed to load texture " << image_name << endl;
    }
    stbi_image_free(data);
}

//OBJECTS

void drawCube(Shader ourShader, glm::mat4 moveMatrix)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model;

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f,0.2f,0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);

    ourShader.setVec4("material.ambient", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);

    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

//person
void drawPerson(Shader ourShader, unsigned int VAO)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;

    translate_X = 6.5f;
    translate_Y = 0.5f;
    translate_Z = -10.0f;

    translateMatrix = glm::translate(identityMatrix, glm::vec3(camera.Position.x, 0.5, camera.Position.z-6));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1, 2.4, 0.3));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(person_rotate), glm::vec3(0.0f, 1.0f, 0.0f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 0.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 0.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 0.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}


void drawFloor(Shader ourShader, unsigned int VAO)
{

  
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;


    //ground
    translate_X = -10;
    translate_Y = 0;
    translate_Z = -40;

    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(80, 0.3, 80));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



    //base 1
    translate_X = -5.0;
    translate_Y = 0.15;
    translate_Z = -35;

    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(60, 0.3, 40));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(0.90, 0.90, 0.90, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(0.90, 0.90, 0.90, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(0.1f, 0.1f, 0.1f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //base 2
    translate_X = -4.0;
    translate_Y = 0.3;
    translate_Z = -34;

    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(46, 0.3, 36));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //roof 1
    translate_X = -4.0;
    translate_Y = 0.3;
    translate_Z = -34;

    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X-2.5, translate_Y+3.75, translate_Z));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(56, 0.3, 24.5));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //

}

   


void drawGate(Shader ourShader, Shader bulbShader, unsigned int VAO)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, translateMatrix1, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, mirror;


    translate_X = 6.5f;
    translate_Y = 0.02f;
    translate_Z = -15.0f;

    //dome 1
    Sphere dome_gate_1 = Sphere(2.0f,-0.5f, 36, 18, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 1, translate_Y + 3.7, translate_Z + 11.175));
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    dome_gate_1.drawSphere(ourShader, translateMatrix);

    // road
    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4, 0.3, 24));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(0.90, 0.90, 0.90, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(0.90, 0.90, 0.90, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(0.1f, 0.1f, 0.1f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //gate walls 
    for (int i = 0;i < 2;i++)
    {

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X-2+i*5, translate_Y, translate_Z+10 ));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2, 6, 5));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[6]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    }

    //gate roof

    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X-2.5 , translate_Y+3, translate_Z + 9));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(14, .5, 8));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBindVertexArray(cubeVAO11);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X - 3, translate_Y + 3.25, translate_Z + 8.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(16, .5, 10));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBindVertexArray(cubeVAO11);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    // ***********  boundary  ************
    for (int i = 0;i < 4;i++)
    {
        //pillar 
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X - 6 - 4 * i, translate_Y, translate_Z + 12.05));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1, 4, 1));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //light stand
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X - 6.15 - 4 * i, translate_Y + 2, translate_Z + 12.10));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5, 0.15, 1.25));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(0.5, 0.5, 0.5, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(0.5, 0.5, 0.5, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(0.5, 0.5, 0.5, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //bulb
        Sphere dome_gate_2 = Sphere(0.3f, -10.0f, 36, 18, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X - 5.75 - 4 * i, translate_Y + 2.3, translate_Z + 12.25));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        dome_gate_2.drawSphere(ourShader, translateMatrix);

        //borderwall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X - 6-4*i, translate_Y, translate_Z + 12));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(8, 3.5, 0.5));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



        


       //mirroring
        
        //pillar 
        translateMatrix = glm::translate(identityMatrix, glm::vec3(15-(translate_X - 6 - 4 * i), translate_Y, translate_Z + 12.05));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1, 4, 1));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //light stand
        translateMatrix = glm::translate(identityMatrix, glm::vec3(15-(translate_X - 5.87 - 4 * i), translate_Y + 2, translate_Z + 12.10));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5, 0.15, 1.25));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(0.5, 0.5, 0.5, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(0.5, 0.5, 0.5, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(0.5, 0.5, 0.5, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //bulb
        Sphere dome_gate_3 = Sphere(0.3f, -10.0f, 36, 18, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(15-(translate_X - 6.25 - 4 * i), translate_Y + 2.3, translate_Z + 12.25));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        dome_gate_3.drawSphere(ourShader, translateMatrix);
        //borderwall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(11-(translate_X - 6 - 4 * i), translate_Y, translate_Z + 12));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(8, 3.5, 0.5));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    }
    
}


void drawWalls(Shader ourShader)
{
    //left front wall
    glBindVertexArray(cubeVAO51);
    translate_X = -4.0;
    translate_Y = 0.3;
    translate_Z = -34;
   
    
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;


    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(20, 6, 0.3));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //right front wall
    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X+13, translate_Y, translate_Z));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    
    glBindVertexArray(cubeVAO51);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //mihrab
    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 10.5, translate_Y, translate_Z-2));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4, 6, 0.3));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glBindVertexArray(cubeVAO11);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //mihrab wall left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 10.5, translate_Y, translate_Z-2));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 6, 4.5));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glBindVertexArray(cubeVAO11);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //mihrab wall right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 12.3, translate_Y, translate_Z - 1.8));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 6, 4.5));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glBindVertexArray(cubeVAO11);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //mihrab floor
    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 10, translate_Y, translate_Z - 2));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(6, 0.3, 4.5));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBindVertexArray(cubeVAO11);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //mihrab roof top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 9, translate_Y+3, translate_Z - 3));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(10, 0.3, 6));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBindVertexArray(cubeVAO11);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // mehrab dome 1
    Sphere dome_gate_1 = Sphere(1.25f, -0.20f, 36, 18, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 11.55 , translate_Y + 3.2, translate_Z -1.5));
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    dome_gate_1.drawSphere(ourShader, translateMatrix);
   

    //front top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y+3, translate_Z));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(46, 1.5, 0.3));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glBindVertexArray(cubeVAO51);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //front top 2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y + 3, translate_Z+12));
    model = translateMatrix *  scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glBindVertexArray(cubeVAO51);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //left top 
    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y+3, translate_Z));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 1.5, 25));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glBindVertexArray(cubeVAO51);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //right top 
    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X+22.7, translate_Y + 3, translate_Z));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 1.5, 25));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glBindVertexArray(cubeVAO51);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //leftwall 1
    for (int i = 0;i < 3;i++)
    {
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z+5*i));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 6, 5));
        model = translateMatrix  * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[6]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X+22.7, translate_Y, translate_Z+5*i));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 6, 5));
        model = translateMatrix  * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[6]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    //outer front 1
    for (float i = 0;i < 6;i++)
    {
        if (i == 3) i += 0.3;
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X+i*3.9, translate_Y, translate_Z+12 ));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.4, 6, 0.3));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[6]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    }


    //outer front 2                                                          x+4+14.8*i 
    for (int i = 0;i < 4;i++)
    {
        if (i % 2 == 0)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 4 + 14.8 * i / 2, translate_Y, translate_Z + 12));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 6, 6));
        }
        if (i % 2 == 1)
        {
           translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 10 + (i-1)*1.4, translate_Y, translate_Z + 12));
           scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 6, 1.5));
        }
       
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    //out rooms
    for (int i = 0;i < 2;i++)
    {

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 10 + i * 2.8, translate_Y, translate_Z + 14));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 6, 1.5));
       

        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //outer top 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 4.1 + i * 8.8, translate_Y + 2.5, translate_Z + 14.6));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(12, 1, 0.3));


        model = translateMatrix  * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //outer roof
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 4 + i * 8.8, translate_Y + 2.9, translate_Z + 12));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(12.3, 0.3,6));


        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //dome 1
        Sphere dome_gate_1 = Sphere(1.5f, -0.20f, 36, 18, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 7 + i * 8.8, translate_Y + 3.0, translate_Z + 13.5));
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        dome_gate_1.drawSphere(ourShader, translateMatrix);
    }    
}





void drawDoor(Shader ourShader)
{
    //left front wall
    translate_X = -4.0;
    translate_Y = 0.3;
    translate_Z = -34;


    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;

    if (camera.Position.x >= 6 && camera.Position.x <= 9 && camera.Position.z < -15 && camera.Position.z >-29)
    {
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 10-(-camera.Position.z-15)*0.4, translate_Y, translate_Z + 11.9));
    }
    else
    {
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 10 , translate_Y, translate_Z + 11.9));
    }
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3, 6, 0.4));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    glBindVertexArray(cubeVAO11);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    if (camera.Position.x >= 6 && camera.Position.x <= 9 && camera.Position.z < -15 && camera.Position.z >-29)
    {
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 11.5 + (-camera.Position.z - 15) * 0.4, translate_Y, translate_Z + 11.9));
    }
    else
    {
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 11.5, translate_Y, translate_Z + 11.9));
    }
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3, 6, 0.4));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
    ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    glBindVertexArray(cubeVAO11);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}

void drawMinar(Shader ourShader)
{
    translate_X = -4.0;
    translate_Y = 0.3;
    translate_Z = -32;

    //pillar 1

    for (int i = 0;i < 2;i++)
    {
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X+i*23, translate_Y, translate_Z + 13));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1, 7.5, 1));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[10]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        //pillar1 cross
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X - 0.11 +i*23, translate_Y, translate_Z + 13.22));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1, 7.5, 1));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = translateMatrix * rotateYMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[10]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    }
}



void drawBookShelf(Shader ourShader)
{
    translate_Y = -0.1;
    translate_Z = -34;
    for (int j = 0;j < 2;j++)
    {
        translate_X = -5.50+j*17.5;

        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;

        //back
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 3, translate_Y + 0.5, translate_Z + 0.2));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.9, 6.5, 0.1));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        //cabinet
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 3, translate_Y + 0.5, translate_Z + 0.2));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(5, 2, 0.6));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[12]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //top
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 3, translate_Y + 3.5, translate_Z + 0.2));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(5, 1, 0.6));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
        ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
        ourShader.setFloat("material.shininess", 32.0f);
        glBindTexture(GL_TEXTURE_2D, texture[12]);
        glBindVertexArray(cubeVAO11);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //self 1
        for (int i = 0;i < 4;i++)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 3, translate_Y + 1.5 + i * 0.5, translate_Z + 0.2));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.9, 0.1, 0.4));
            model = translateMatrix * scaleMatrix;
            ourShader.setMat4("model", model);

            //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
            ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
            ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
            ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
            ourShader.setFloat("material.shininess", 32.0f);
            glBindTexture(GL_TEXTURE_2D, texture[0]);
            glBindVertexArray(cubeVAO11);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            //book
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 3.1, translate_Y + 1.55 + i * 0.5, translate_Z + 0.2));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.7, 0.95, 0.3));
            model = translateMatrix * scaleMatrix;
            ourShader.setMat4("model", model);

            //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
            ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
            ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
            ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
            ourShader.setFloat("material.shininess", 32.0f);
            glBindTexture(GL_TEXTURE_2D, texture[14 + i % 2]);
            glBindVertexArray(cubeVAO11);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        }

        //side
        for (int i = 0;i < 3;i++)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 3 + i * 1.22, translate_Y + 0.5, translate_Z + 0.18));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 6.5, 0.6));
            model = translateMatrix * scaleMatrix;
            ourShader.setMat4("model", model);

            //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
            ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
            ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
            ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
            ourShader.setFloat("material.shininess", 32.0f);
            glBindTexture(GL_TEXTURE_2D, texture[12]);
            glBindVertexArray(cubeVAO11);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }


        //door1       
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 3, translate_Y + 1.5, translate_Z + 0.48));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5, 4, 0.1));

            if (camera.Position.x >= translate_X + 3 && camera.Position.x <= translate_X + 8 && camera.Position.z > translate_Z && camera.Position.z < translate_Z + 5)
            {
              
                if (k <= 135) {
                    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-k), glm::vec3(0.0f, 1.0f, 0.0f));
                    k++;
                }
                else {
                    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                }
            }
            
            model = translateMatrix * rotateYMatrix * scaleMatrix;

            ourShader.setMat4("model", model);

            //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
            ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
            ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
            ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
            ourShader.setFloat("material.shininess", 32.0f);
            glBindTexture(GL_TEXTURE_2D, texture[12]);
            glBindVertexArray(cubeVAO11);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        //door 2
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X + 3+2.5, translate_Y + 1.5, translate_Z + 0.53));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5, 4, 0.1));

            if (camera.Position.x >= translate_X + 3 && camera.Position.x <= translate_X + 8 && camera.Position.z > translate_Z && camera.Position.z < translate_Z + 5)
            {

                if (k <= 135) {
                    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-180+k), glm::vec3(0.0f, 1.0f, 0.0f));
                    k++;
                }
                else {
                    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-180+135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                }
            }

            model = translateMatrix * rotateYMatrix * scaleMatrix;
            ourShader.setMat4("model", model);

            //ourShader.setVec4("color", glm::vec4(0.970, 0.757, 0.757, 1.0f));
            ourShader.setVec4("material.ambient", glm::vec4(1, 1, 1, 1.0f));
            ourShader.setVec4("material.diffuse", glm::vec4(1, 1, 1, 1.0f));
            ourShader.setVec4("material.specular", glm::vec4(1, 1, 1, 1.0f));
            ourShader.setFloat("material.shininess", 32.0f);
            glBindTexture(GL_TEXTURE_2D, texture[12]);
            glBindVertexArray(cubeVAO11);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
    }
}