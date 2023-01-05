#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types
#include <glm/gtx/transform.hpp>
#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>

// window
gps::Window myWindow;

int retina_width, retina_height;
const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

glm::vec3 lightPosF1;
glm::vec3 lightPosF2;
glm::vec3 lightPosF3;
glm::vec3 lightPosF4;
glm::vec3 lightPosF5;
glm::vec3 lightColorF;

// shader uniform locations
GLuint modelLoc;
GLuint viewLoc;
GLuint projectionLoc;
GLuint normalMatrixLoc;
GLuint lightDirLoc;
GLuint lightColorLoc;
GLuint lightPosF1Loc;
GLuint lightPosF2Loc;
GLuint lightPosF3Loc;
GLuint lightPosF4Loc;
GLuint lightPosF5Loc;
GLuint lightColorFLoc;

gps::SkyBox mySkyBox;
gps::Shader skyBoxShader;


std::vector<const GLchar*> faces;

// camera
gps::Camera myCamera(
    glm::vec3(00.0f, 0.1f, 13.0f),
    glm::vec3(0.0f, 0.0f, 10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.08f;

GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
gps::Model3D ground;
gps::Model3D grocery;
gps::Model3D depot;
gps::Model3D brownstone;
gps::Model3D washington;
gps::Model3D wabasha;
gps::Model3D fairfax;
gps::Model3D tree;
gps::Model3D glass_tower;
gps::Model3D monument;
gps::Model3D montgomery;
gps::Model3D market;
gps::Model3D lenoxstones;
gps::Model3D apartments;
gps::Model3D smallhouse;
gps::Model3D oakschool;
gps::Model3D blackstone;
gps::Model3D lasalleplaza;
gps::Model3D largehouse;
gps::Model3D medicalbuilding;
gps::Model3D mediumhouse;
gps::Model3D car;
gps::Model3D dog;
gps::Model3D grass;
gps::Model3D airplane;
gps::Model3D streetlamp;
gps::Model3D fence;
gps::Model3D wall;
gps::Model3D stones;
gps::Model3D rock;
gps::Model3D sand;
gps::Model3D water;

GLfloat angle;
GLfloat airplaneX = -10.0f;
GLfloat airplaneY;
GLfloat airplaneZ;

glm::mat4 lightRotation;
GLfloat lightAngle;

// shaders
gps::Shader myBasicShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

bool openGates = false;
float gateAngle = 10.0f;
bool firstO = true;
bool incar = false;
float carangle = 0.0f;
glm::vec3 carDir = glm::vec3(0.0f, 0.0f, 1.0f);

glm::vec3 carpos = glm::vec3(4.0f, -1.0f, 12.7f);
bool isDay = true;

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    //std::cout << "xpos = " << xpos << ", ypos = " << ypos << std::endl;
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

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    myCamera.rotate(pitch, yaw);
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

void processMovement() {
    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_UP]) {
        myCamera.move(gps::MOVE_UPWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_DOWN]) {
        myCamera.move(gps::MOVE_DOWNWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
}

void processInput() {
    if (pressedKeys[GLFW_KEY_O]) {
        openGates = true;
        gateAngle = 0.0f;
        firstO = false;
    }

    if (pressedKeys[GLFW_KEY_P]) {
        openGates = false;
        gateAngle = 0.0f;
    }

    if (pressedKeys[GLFW_KEY_F]) {
        incar = true;
        cameraSpeed = 0.14;
    }

    if (pressedKeys[GLFW_KEY_G]) {
        incar = false;
        cameraSpeed = 0.08f;
    }

    if (pressedKeys[GLFW_KEY_Z]) {
        //wireframe on
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (pressedKeys[GLFW_KEY_X]) {
        //wireframe off
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (pressedKeys[GLFW_KEY_C]) {
        //wireframe off
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    if (pressedKeys[GLFW_KEY_N]) {
        isDay = false;
        //set light color
        lightColor = glm::vec3(0.05f, 0.05f, 0.05f); //dark light (night time)
        lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
        // send light color to shader
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    }

    if (pressedKeys[GLFW_KEY_M]) {
        isDay = true;
        //set light color
        lightColor = glm::vec3(0.25f, 0.25f, 0.25f); //bright light (day time)
        lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
        // send light color to shader
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    }
}

void initOpenGLWindow() {
    myWindow.Create(1920, 1080, "OpenGL Project Core");
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);

    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    ground.LoadModel("models/ground/ground.obj");
    grass.LoadModel("models/ground/grass.obj");
    water.LoadModel("models/ground/water.obj");
    sand.LoadModel("models/ground/sand.obj");
    grocery.LoadModel("models/grocery/adamgrocery.obj");
    brownstone.LoadModel("models/brownstones/lenoxbrownstones.obj");
    depot.LoadModel("models/depot/milwaukeeroaddepot.obj");
    washington.LoadModel("models/washington/317washington.obj");
    wabasha.LoadModel("models/wabasha/386wabasha.obj");
    fairfax.LoadModel("models/fairfax/fairfaxparsons.obj");
    glass_tower.LoadModel("models/glass_tower/northwesternmutual.obj");
    monument.LoadModel("models/monument/2800monument.obj");
    montgomery.LoadModel("models/montgomery/montgomerytower.obj");
    market.LoadModel("models/market/westsidemarket.obj");
    lenoxstones.LoadModel("models/lenoxstones/lenoxbrownstones.obj");
    apartments.LoadModel("models/apartments/mplsapt1.obj");
    smallhouse.LoadModel("models/smallhouse/farmhouse1.obj");
    mediumhouse.LoadModel("models/mediumhouse/farmhouse2.obj");
    largehouse.LoadModel("models/largehouse/farmhouse3.obj");
    oakschool.LoadModel("models/oakschool/oakview.obj");
    blackstone.LoadModel("models/blackstoneap/blackstone.obj");
    lasalleplaza.LoadModel("models/lasalleplaza/lasalleplaza.obj");
    medicalbuilding.LoadModel("models/medicalbuilding/loringmedical.obj");
    car.LoadModel("models/car/Jeep_Renegade_2016.obj");
    dog.LoadModel("models/dog/12228_Dog_v1_L2.obj");
    airplane.LoadModel("models/airplane/11803_Airplane_v1_l1.obj");
    streetlamp.LoadModel("models/streetlamp/streetlamp.obj");
    fence.LoadModel("models/fence/Fence_Game_Asset_02.obj");
    wall.LoadModel("models/wall/wall.obj");
    stones.LoadModel("models/stones/StonesLowPoly.obj");
    rock.LoadModel("models/rock/Stone_multi_p1_Complete.obj");
    tree.LoadModel("models/tree/Tree.obj");

    faces.push_back("skybox/right.jpg");
    faces.push_back("skybox/left.jpg");
    faces.push_back("skybox/top.jpg");
    faces.push_back("skybox/bottom.jpg");
    faces.push_back("skybox/back.jpg");
    faces.push_back("skybox/front.jpg");
    mySkyBox.Load(faces);
}

void initShaders() {
    myBasicShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
    skyBoxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
}

glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix
    glm::mat4 lightView = glm::lookAt(glm::mat3(lightRotation) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.1f, far_plane = 5.0f;
    glm::mat4 lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix;
}

void initUniforms() {
    myBasicShader.useShaderProgram();
    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(fov),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 100.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //THIS IS THE DIRECTIONAL LIGHT
    lightDir = glm::vec3(-5.5f, 2.0f, 6.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    //set light color
    lightColor = glm::vec3(0.2f, 0.2f, 0.2f); //dark light (night time)
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    //FELINARUL1
    lightPosF1 = glm::vec3(-5.5f, 0.2f, 4.0f); 
    lightPosF1Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPosF1");
    glUniform3fv(lightPosF1Loc, 1, glm::value_ptr(lightPosF1));

    //FELINARUL2
    lightPosF2 = glm::vec3(1.5f, 0.2f, 5.5f);
    lightPosF2Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPosF2");
    glUniform3fv(lightPosF2Loc, 1, glm::value_ptr(lightPosF2));

    //FELINARUL3
    lightPosF3 = glm::vec3(7.5f, 0.2f, 6.0f);
    lightPosF3Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPosF3");
    glUniform3fv(lightPosF3Loc, 1, glm::value_ptr(lightPosF3));

    //FELINARUL4
    lightPosF4 = glm::vec3(13.5f, 0.2, 6.0f);
    lightPosF4Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPosF4");
    glUniform3fv(lightPosF4Loc, 1, glm::value_ptr(lightPosF4));

    //FELINARUL5
    lightPosF5 = glm::vec3(19.5f, 0.2, 6.0f);
    lightPosF5Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPosF5");
    glUniform3fv(lightPosF5Loc, 1, glm::value_ptr(lightPosF5));

    //COLOR FOR STREET LAMPS
    lightColorF = glm::vec3(1.0f, 1.0f, 0.2f); //yellow light
    lightColorFLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColorF");
    // send light color to shader
    glUniform3fv(lightColorFLoc, 1, glm::value_ptr(lightColorF));
}

void initFBO() {
    //generate FBO ID
    glGenFramebuffers(1, &shadowMapFBO);
    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderGround(gps::Shader shader) {
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(4.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    ground.Draw(shader);
}

void renderParkGrass(gps::Shader shader) {
    //add 4.8 to each x of each translate
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.998f, 8.6f));
    model = glm::scale(model, glm::vec3(0.24f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    grass.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(-10.5f, -0.998f, 24.0f));
    model = glm::scale(model, glm::vec3(0.24f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    grass.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.7f, -0.998f, 24.0f));
    model = glm::scale(model, glm::vec3(0.24f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    grass.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.9f, -0.998f, 24.0f));
    model = glm::scale(model, glm::vec3(0.24f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    grass.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(3.9f, -0.998f, 24.0f));
    model = glm::scale(model, glm::vec3(0.24f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    grass.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(8.7f, -0.998f, 24.0f));
    model = glm::scale(model, glm::vec3(0.24f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    grass.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(13.5f, -0.998f, 24.0f));
    model = glm::scale(model, glm::vec3(0.24f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    grass.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(18.3f, -0.998f, 24.0f));
    model = glm::scale(model, glm::vec3(0.24f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    grass.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(11.85f, -0.998f, 8.64f));
    model = glm::scale(model, glm::vec3(0.24f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    grass.Draw(shader);
}

void renderWater(gps::Shader shader) {
    //add 4.8 to each x of each translate

    float x;
    float z = 38.4f;
    for (int i = 0; i < 1; i++) {
        x = -10.5;
        for (int j = 0; j < 7; j++) {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(x, -0.998f, z));
            model = glm::scale(model, glm::vec3(0.24f));
            glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            water.Draw(shader);
            x += 4.8f;
        }
        z += 4.8f;
    }

}

void renderSand(gps::Shader shader) {
    //add 4.8 to each x of each translate

    float x = -10.5;
    float z = 28.8f;
    for (int j = 0; j < 7; j++) {
        model = glm::translate(glm::mat4(1.0f), glm::vec3(x, -0.998f, z));
        model = glm::scale(model, glm::vec3(0.24f));
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        sand.Draw(shader);
        x += 4.8f;
    }
    x = -10.5;
    z = 33.6f;
    for (int j = 0; j < 7; j++) {
        model = glm::translate(glm::mat4(1.0f), glm::vec3(x, -0.998f, z));
        model = glm::scale(model, glm::vec3(0.24f));
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        sand.Draw(shader);
        x += 4.8f;
    }

}

void renderGrocery(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, -1.0f, -0.5f));
    model = glm::scale(model, glm::vec3(0.0015f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    grocery.Draw(shader);
}

void renderBrownstones(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(1.35f, -1.0f, -0.4f));
    model = glm::scale(model, glm::vec3(0.0015f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    brownstone.Draw(shader);
}

void renderDepot(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(4.65f, -1.0f, 1.05f));
    model = glm::scale(model, glm::vec3(0.0013f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    depot.Draw(shader);
}

void renderWashington(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.45f, -1.0f, 0.65f));
    model = glm::scale(model, glm::vec3(0.0012f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    washington.Draw(shader);
}

void renderWabasha(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.45f, -1.0f, 8.65f));
    model = glm::scale(model, glm::vec3(0.0007f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    wabasha.Draw(shader);
}

void renderFairfax(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(3.45f, -1.0f, 8.65f));
    model = glm::scale(model, glm::vec3(0.0015f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    fairfax.Draw(shader);

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(9.01f, -1.0f, 8.65f));
    model = glm::scale(model, glm::vec3(0.0015f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    fairfax.Draw(shader);
}

void renderTree(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();
    //left park
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, 7.0f));
    model = glm::scale(model, glm::vec3(0.225f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    tree.Draw(shader);

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, 8.0f));
    model = glm::scale(model, glm::vec3(0.225f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    tree.Draw(shader);

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, 10.0f));
    model = glm::scale(model, glm::vec3(0.225f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    tree.Draw(shader);

    //right park
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(13.5f, -1.0f, 7.0f));
    model = glm::scale(model, glm::vec3(0.225f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    tree.Draw(shader);

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(13.5f, -1.0f, 8.0f));
    model = glm::scale(model, glm::vec3(0.225f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    tree.Draw(shader);

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(13.5f, -1.0f, 10.0f));
    model = glm::scale(model, glm::vec3(0.225f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    tree.Draw(shader);

}

void renderGlassTower(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(20.45f, -1.0f, 9.5f));
    model = glm::scale(model, glm::vec3(0.00035f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    glass_tower.Draw(shader);
}

void renderMonument(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(6.3f, -1.0f, 10.5f));
    model = glm::scale(model, glm::vec3(0.002f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    monument.Draw(shader);
}

void renderSkyBox(gps::Shader shader) {
    shader.useShaderProgram();

    mySkyBox.Draw(shader, view, projection);
}

void renderMontgomery(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.1f, -1.0f, 9.5f));
    model = glm::scale(model, glm::vec3(0.0006f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    montgomery.Draw(shader);
}

void renderMarket(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-10.5f, -1.0f, 6.3f));
    model = glm::scale(model, glm::vec3(0.00132));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    market.Draw(shader);
}

void renderLenoxstones(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(7.94, -1.0f, -0.3f));
    model = glm::scale(model, glm::vec3(0.00143));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    lenoxstones.Draw(shader);
}

void renderApartments(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(10.5f, -1.0f, -2.5f));
    model = glm::scale(model, glm::vec3(0.003));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    apartments.Draw(shader);
}

void renderSmallhouse(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-10.5f, -1.0f, 24.0f));
    model = glm::scale(model, glm::vec3(0.002));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    smallhouse.Draw(shader);

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -1.0f, 24.0f));
    model = glm::scale(model, glm::vec3(0.002));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    smallhouse.Draw(shader);

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(14.5f, -1.0f, 24.0f));
    model = glm::scale(model, glm::vec3(0.002));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    smallhouse.Draw(shader);
}

void renderMediumhouse(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(4.5f, -1.0f, 24.0f));
    model = glm::scale(model, glm::vec3(0.002));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    mediumhouse.Draw(shader);

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(19.5f, -1.0f, 24.0f));
    model = glm::scale(model, glm::vec3(0.002));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    mediumhouse.Draw(shader);
}

void renderLargehouse(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.5f, -1.0f, 24.0f));
    model = glm::scale(model, glm::vec3(0.002));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    largehouse.Draw(shader);

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(9.5f, -1.0f, 24.0f));
    model = glm::scale(model, glm::vec3(0.002));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    largehouse.Draw(shader);
}

void renderBlackstoneAp(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(12.6f, -1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.002));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    blackstone.Draw(shader);
}

void renderLasalleplaza(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(15.7f, -1.0f, 9.5f));
    model = glm::scale(model, glm::vec3(0.00048));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    lasalleplaza.Draw(shader);
}

void renderOakschool(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(6.5f, -1.0f, -20.0f));
    model = glm::scale(model, glm::vec3(0.0027));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    oakschool.Draw(shader);
}

void renderMedicalBuilding(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(17.5f, -1.0f, 0.4f));
    model = glm::scale(model, glm::vec3(0.0017));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    medicalbuilding.Draw(shader);
}

void renderCar(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    //model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 14.0f));
    if (incar == true) {
        carpos = myCamera.cameraPosition;
        carpos.y = -1.0f;
        glm::vec3 frontProjection = normalize(myCamera.cameraFrontDirection - glm::dot(myCamera.cameraFrontDirection, glm::vec3(0.0f, 0.1f, 0.0f)) * glm::vec3(0.0f, 0.1f, 0.0f));
        glm::vec3 dir = glm::cross(frontProjection, carDir);
        float dot = glm::dot(carDir, myCamera.cameraFrontDirection);
        carangle = acos(dot);
        if (dir.y > 0.0f) {
            carangle = 270.0f - carangle;
        }
        model = glm::translate(glm::mat4(1.0f), carpos);
        model = glm::scale(model, glm::vec3(0.5f));
        model = glm::rotate(model, carangle, glm::vec3(0.0f, 1.0f, 0.0f)); //trebuie si rotita sa arate bine
    }
    else {
        model = glm::translate(glm::mat4(1.0f), carpos);
        model = glm::scale(model, glm::vec3(0.5f));
        model = glm::rotate(model, carangle, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    car.Draw(shader);
}

void renderAirplane(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(airplaneX, 18.0f, -0.1f));
    model = glm::scale(model, glm::vec3(0.0018f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.2f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    airplaneX = airplaneX + 0.02f;
    airplane.Draw(shader);
}

void renderDog(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");

    model = glm::translate(glm::mat4(1.0f), glm::vec3(1.35f, -1.0f, 7.4f));
    model = glm::scale(model, glm::vec3(0.007f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    dog.Draw(shader);
}

void renderStreetlamp(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    float x = -5.5f;
    float z = 6.0f;
    for (int i = 0; i < 5; i++) {
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        modelLoc = glGetUniformLocation(shader.shaderProgram, "model");

        model = glm::translate(glm::mat4(1.0f), glm::vec3(x, -1.0f, z));
        model = glm::scale(model, glm::vec3(0.14f));
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        //send teapot normal matrix data to shader
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

        streetlamp.Draw(shader);
        x += 6.0f;
    }
}

void renderFence(gps::Shader shader) {
    // select active shader program
    float x = -12.8;
    shader.useShaderProgram();

    for (int i = 0; i < 42; i++) {

        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
        model = glm::translate(glm::mat4(1.0f), glm::vec3(x, -1.0f, 21.6f));
        model = glm::scale(model, glm::vec3(0.01));
        if (openGates == true) {
            if ((i - 3) % 6 == 0) {
                if (gateAngle <= 90.0f) {
                    model = glm::rotate(model, glm::radians(gateAngle), glm::vec3(0.0f, 1.0f, 0.0f));
                    if (gateAngle != 90.0f) gateAngle += 1.0f;
                }
            }
        }
        else
        {
            if(!firstO)
                if ((i - 3) % 6 == 0) {
                    if (gateAngle <= 90.0f) {
                        model = glm::rotate(model, glm::radians(90.0f - gateAngle), glm::vec3(0.0f, 1.0f, 0.0f));
                        if (gateAngle != 90.0f) gateAngle += 1.0f;
                    }
                }
        }
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

        fence.Draw(shader);
        x += 0.8;
    }

    x = -12.8;
    for (int i = 0; i < 8; i++) {
        float z = 22.4f;
        for (int j = 0; j < 6; j++) {
            model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
            modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
            if (i == 7) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(x, -1.0f, z - 0.8f));
                model = glm::scale(model, glm::vec3(0.01));
                model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(x, -1.0f, z));
                model = glm::scale(model, glm::vec3(0.01));
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            fence.Draw(shader);
            z += 0.8;
        }
        x += 4.8;
    }
}

void renderWall(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();
    //aici sunt cei din spate de tot, de la parcul din dreapta
    float x = 2.2;
    for (int i = 0; i < 6; i++) {
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
        model = glm::translate(glm::mat4(1.0f), glm::vec3(x, -1.0f, 6.3f));
        model = glm::scale(model, glm::vec3(0.12));
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

        wall.Draw(shader);
        x -= 1.4f;
    }

    x = 2.2;
    for (int i = 0; i < 6; i++) {
        if (i != 2) {
            model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
            modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
            model = glm::translate(glm::mat4(1.0f), glm::vec3(x, -1.0f, 11.0f));
            model = glm::scale(model, glm::vec3(0.12));
            glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            wall.Draw(shader);

        }
        x -= 1.4f;
    }

    x = 10.3f;
    for (int i = 0; i < 6; i++) {
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
        model = glm::translate(glm::mat4(1.0f), glm::vec3(x, -1.0f, 6.3f));
        model = glm::scale(model, glm::vec3(0.12));
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

        wall.Draw(shader);
        x += 1.4f;
    }

    x = 10.3;
    for (int i = 0; i < 6; i++) {
        if (i != 1) {
            model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
            modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
            model = glm::translate(glm::mat4(1.0f), glm::vec3(x, -1.0f, 11.0f));
            model = glm::scale(model, glm::vec3(0.12));
            glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            wall.Draw(shader);
        }
        x += 1.4f;
    }

    //astea sunt cele verticale
    float z = 10.3f;
    x -= 0.7f;
    for (int i = 0; i < 3; i++) {
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
        model = glm::translate(glm::mat4(1.0f), glm::vec3(x, -1.0f, z));
        model = glm::scale(model, glm::vec3(0.12));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

        wall.Draw(shader);
        z -= 1.4f;
    }

}

void renderStones(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.675f, -0.85f, 26.8f));
    model = glm::scale(model, glm::vec3(0.96));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    stones.Draw(shader);

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(8.7f, -0.8f, 27.0f));
    model = glm::scale(model, glm::vec3(1.0));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    stones.Draw(shader);

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(17.9f, -0.9f, 27.2f));
    model = glm::scale(model, glm::vec3(1.3));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    stones.Draw(shader);

}

void renderRocks(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-10.675f, -0.9f, 29.8f));
    model = glm::scale(model, glm::vec3(0.0046));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    rock.Draw(shader);

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(7.7f, -1.0f, 29.5f));
    model = glm::scale(model, glm::vec3(0.0053));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    rock.Draw(shader);

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    model = glm::translate(glm::mat4(1.0f), glm::vec3(2.9f, -1.0f, 28.6f));
    model = glm::scale(model, glm::vec3(0.0038));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    rock.Draw(shader);

}

void renderObjects(gps::Shader shader) {
    renderGrocery(shader);
    renderBrownstones(shader);
    renderDepot(shader);
    renderWashington(shader);
    renderWabasha(shader);
    renderFairfax(shader);
    renderTree(shader);
    renderGlassTower(shader);
    renderMonument(shader);
    renderMontgomery(shader);
    renderMarket(shader);
    renderLenoxstones(shader);
    renderSmallhouse(shader);
    renderOakschool(shader);
    renderBlackstoneAp(shader);
    renderLasalleplaza(shader);
    renderLargehouse(shader);
    renderMedicalBuilding(shader);
    renderMediumhouse(shader);
    renderAirplane(shader);
    renderDog(shader);
    renderStreetlamp(shader);
    renderCar(shader);
    renderFence(shader);
    renderWall(shader);
    renderStones(shader);
    renderRocks(shader);
    renderTree(shader);
    renderGround(shader);
    renderParkGrass(shader);
    renderWater(shader);
    renderSand(shader);
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //render the scene    
    renderObjects(myBasicShader);
    renderSkyBox(skyBoxShader);
}


void cleanup() {
    myWindow.Delete();
}

int main(int argc, const char* argv[]) {

    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    setWindowCallbacks();

    glCheckError();
    // application loop
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        processInput();
        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());

        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
