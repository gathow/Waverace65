#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#include <vector>
#include <iostream>

#include "tiny_obj_loader.h"
#include "stb_image.h"
#include <shader.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;
int fbWidth, fbHeight;

//Deltatime
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

//Camera

vec3 playerPos = vec3(0.0f, 0.0f, 0.0f);
vec3 followOffset = vec3(0.0f, 4.0f, -8.0f);
vec3 cameraPos   = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, 1.0f);
vec3 cameraUp    = vec3(0.0f, 1.0f,  0.0f);
vec3 up = vec3(0.0f, 1.0f, 0.0f); 
vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
vec3 cameraDirection = normalize(cameraPos - cameraTarget);
vec3 cameraRight = normalize(cross(up, cameraDirection));
bool firstMouse = true;
float cameraYaw   = -90.0f;
float cameraPitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

struct Vertex {
    float x, y, z;
    float u, v;
};

struct Object {
    string name;
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int texture = 0;
    GLsizei vertexCount = 0;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

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

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    cameraYaw += xoffset;
    cameraPitch += yoffset;

    // make sure that when cameraPitch is out of bounds, screen doesn't get flipped
    if (cameraPitch > 89.0f)
        cameraPitch = 89.0f;
    if (cameraPitch < -89.0f)
        cameraPitch = -89.0f;

    vec3 front;
    front.x = cos(radians(cameraYaw)) * cos(radians(cameraPitch));
    front.y = sin(radians(cameraPitch));
    front.z = sin(radians(cameraYaw)) * cos(radians(cameraPitch));
    vec3 cameraFront = normalize(vec3(0.0f, -0.25f, -1.0f));

}

void processInput(GLFWwindow *window)
{

vec3 direction;
direction.x = cos(radians(cameraYaw)) * cos(radians(cameraPitch));
direction.y = sin(radians(cameraPitch));
direction.z = sin(radians(cameraYaw)) * cos(radians(cameraPitch));


    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float playerSpeed = 2.5f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS)
            playerSpeed *= 4.0f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            playerPos += playerSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            playerPos -= playerSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            playerPos -= normalize(cross(cameraFront, cameraUp)) * playerSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            playerPos += normalize(cross(cameraFront, cameraUp)) * playerSpeed; 

}

Object CreateObject(
    const string& objectName,
    const string& modelFile,
    const string& textureFile)
{
    Object object;
    object.name = objectName;

    tinyobj::ObjReader reader;
    reader.ParseFromFile(modelFile);

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();
    vector<Vertex> vertices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            if (index.vertex_index < 0)
                continue;

            const size_t p = 3 * static_cast<size_t>(index.vertex_index);
            Vertex vertex{};
            vertex.x = attrib.vertices[p];
            vertex.y = attrib.vertices[p + 1];
            vertex.z = attrib.vertices[p + 2];

            if (index.texcoord_index >= 0) {
                const size_t t = 2 * static_cast<size_t>(index.texcoord_index);
                vertex.u = attrib.texcoords[t];
                vertex.v = attrib.texcoords[t + 1];
            }

            vertices.push_back(vertex);
        }
    }
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(
        textureFile.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    glGenTextures(1, &object.texture);
    glBindTexture(GL_TEXTURE_2D, object.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    glGenVertexArrays(1, &object.VAO);
    glGenBuffers(1, &object.VBO);

    glBindVertexArray(object.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, object.VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    object.vertexCount = static_cast<GLsizei>(vertices.size());
    return object;
}

void drawObject(const Object& object,GLint modelLoc, float deltaX, float deltaY, float deltaZ) {
    mat4 modelPos = translate(mat4(1.0f), vec3(deltaX, deltaY, deltaZ));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(modelPos));
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, object.texture);
    glBindVertexArray(object.VAO);
    glDrawArrays(GL_TRIANGLES, 0, object.vertexCount);
    glBindVertexArray(0);
}

//MAIN CODE
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

GLFWwindow* window = glfwCreateWindow(800, 400, "WaveRace65", NULL, NULL);
glfwMakeContextCurrent(window);
gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
 
glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
glfwSetCursorPosCallback(window, mouse_callback);
glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
glViewport(0, 0, fbWidth, fbHeight);

//MODEL LOADING
Shader ourShader("vertexshader.vs", "fragmentshader.fs");
Object tuna = CreateObject("Tuna", "Tuna-bl.obj", "Tuna_texture.png");
Object cube = CreateObject("Cube", "cube.obj", "Zuko.jpg");

//All rendering in here
while(!glfwWindowShouldClose(window))
{
//Time calculation
float currentFrame = glfwGetTime();
deltaTime = currentFrame - lastFrame;
lastFrame = currentFrame;  

//input
processInput(window);

//Shader
ourShader.use();
ourShader.setInt("texture1", 0);

//View Matrix

mat4 model = translate(mat4(1.0f), playerPos);
mat4 viewmatrix    = mat4(1.0f);
mat4 perspectiveprojection    = mat4(1.0f);

cameraPos = playerPos + followOffset;
viewmatrix = lookAt(cameraPos, playerPos, cameraUp);
perspectiveprojection = perspective(radians(45.0f), static_cast<float>(fbWidth) / static_cast<float>(fbHeight), 0.1f, 100.0f);

//Transformation matrixes -> Shader
int modelLoc = glGetUniformLocation(ourShader.ID, "model");
int viewLoc = glGetUniformLocation(ourShader.ID, "viewmatrix");
glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(viewmatrix));
int projectionLoc = glGetUniformLocation(ourShader.ID, "perspectiveprojection");
glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(perspectiveprojection)); 

//Rendering
glEnable(GL_DEPTH_TEST); 
glClearColor(0.3490f, 0.7569f, 0.9804f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

ourShader.use();
drawObject(tuna, modelLoc, playerPos.x, playerPos.y, playerPos.z);
drawObject(cube, modelLoc, 2.0f, 1.0f, 0.0f);
//Buffer swap
glfwSwapBuffers(window);
glfwPollEvents();    

}

glfwTerminate();
return 0;
}
