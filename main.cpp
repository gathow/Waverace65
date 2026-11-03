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

int fbWidth, fbHeight;

//Deltatime
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

//Camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

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

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow *window)
{

glm::vec3 direction;
direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
direction.y = sin(glm::radians(pitch));
direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));


    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

GLFWwindow* window = glfwCreateWindow(800, 400, "LearnOpenGL", NULL, NULL);
glfwMakeContextCurrent(window);

gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
 
glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
glfwSetCursorPosCallback(window, mouse_callback);
glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
glViewport(0, 0, fbWidth, fbHeight);

//MODEL LOADING
tinyobj::ObjReader reader;

if (!reader.ParseFromFile("Tuna-bl.obj")) {
    cerr << "OBJ load error: " << reader.Error() << '\n';
}

const auto& attrib = reader.GetAttrib();
const auto& shapes = reader.GetShapes();

//READING MODEL VERTEX
struct Vertex {
    float x, y, z; // position
    float u, v;    // texture coordinate
};

vector<Vertex> vertices;

for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
        int p = 3 * index.vertex_index; // 3 position numbers: x, y, z

        Vertex vertex{};
        vertex.x = attrib.vertices[p + 0];
        vertex.y = attrib.vertices[p + 1];
        vertex.z = attrib.vertices[p + 2];

        if (index.texcoord_index >= 0) {
            int t = 2 * index.texcoord_index; // 2 UV numbers: u, v
            vertex.u = attrib.texcoords[t + 0];
            vertex.v = attrib.texcoords[t + 1];
        }

        vertices.push_back(vertex);
    }
}


int width, height, nrChannels;
stbi_set_flip_vertically_on_load(true); 
//unsigned char *data = stbi_load("Zuko.jpg", &width, &height, &nrChannels, 0); 
unsigned char *data = stbi_load("Tuna_texture.png", &width, &height, &nrChannels, STBI_rgb_alpha);
Shader ourShader("vertexshader.vs", "fragmentshader.fs");

//Setting Up drawing
unsigned int texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);  
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
glGenerateMipmap(GL_TEXTURE_2D);
stbi_image_free(data);

unsigned int VAO, VBO;
    
glGenVertexArrays(1, &VAO);  
glGenBuffers(1, &VBO); 

glBindVertexArray(VAO);  
glBindBuffer(GL_ARRAY_BUFFER, VBO);

glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);    
   
// position attribute
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
// texture coord attribute
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);
    
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

glm::mat4 model         = glm::mat4(1.0f);
glm::mat4 viewmatrix    = glm::mat4(1.0f);
glm::mat4 perspectiveprojection    = glm::mat4(1.0f);

viewmatrix =glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
perspectiveprojection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

//Transformation matrixes -> Shader
int modelLoc = glGetUniformLocation(ourShader.ID, "model");
glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
int viewLoc = glGetUniformLocation(ourShader.ID, "viewmatrix");
glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewmatrix));
int projectionLoc = glGetUniformLocation(ourShader.ID, "perspectiveprojection");
glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(perspectiveprojection)); 

//Rendering
glEnable(GL_DEPTH_TEST); 
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
glClearColor(0.3490f, 0.7569f, 0.9804f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT);

glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, texture);

glBindVertexArray(VAO);

glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
glBindVertexArray(0);

//Buffer swap
glfwSwapBuffers(window);
glfwPollEvents();    
}

glfwTerminate();
return 0;
}
