/*
Benjamin Abbott
CS 330
Comp Graphic and Visualization 22EW
*/


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

//GLM libs
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//SOIL 
#include <SOIL2.h>

using namespace std;

const double PI = 3.14159;
const float toRadians = PI / 180.0f;
int width = 1200;
int height = 800;

// Input Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void process_input(GLFWwindow* window);

// Declare View matrix
glm::mat4 viewMatrix;

// initialize field of view
GLfloat fov = 45.0f;

// Define camera attribs
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFrontBack = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraLeftRight = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraUpDown = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraPosition - target);
glm::vec3 direction;

// Camera transformation prototype
void TransformCamera();

// Bool for keys and mouse buttons
bool keys[1024];
bool mouseButtons[3];

// Bool to check camera transformations
GLfloat radius = 3.0f;
GLfloat yaw = 0.0f;
GLfloat pitch = 0.0f;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat lastX = width / 2;
GLfloat lastY = height / 2;
GLfloat xChange;
GLfloat yChange;
float cameraSpeed = 0.001f;
bool firstMouseMove = true;
bool isOrbiting = false;

// Light Source Position
glm::vec3 lightPosition(3.0f, 1.0f, 1.0f);

void draw()
{
    GLenum mode = GL_QUADS;

    GLsizei indices = 4;
    glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);

}
void drawBottle()
{
    GLenum mode = GL_TRIANGLES;

    //number of vertices
    GLsizei indices = 72;
    glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}

void drawBottleLeft()
{
    GLenum mode = GL_QUADS;

    //number of vertices
    GLsizei indices = 4;
    glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}

void drawBottleFront()
{
    GLenum mode = GL_QUADS;

    //number of vertices
    GLsizei indices = 4;
    glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}

void drawBottleBack()
{
    GLenum mode = GL_QUADS;

    //number of vertices
    GLsizei indices = 8;
    glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}

void drawBottleRight()
{
    GLenum mode = GL_QUADS;

    //number of vertices
    GLsizei indices = 4;
    glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}

void drawBottleTop()
{
    GLenum mode = GL_TRIANGLES;

    //number of vertices
    GLsizei indices = 18;
    glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}

void drawPaintCan()
{
    GLenum mode = GL_QUADS;

    //number of vertices
    GLsizei indices = 32;
    glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}

void drawPaintCanTop()
{
    GLenum mode = GL_TRIANGLES;

    //number of vertices
    GLsizei indices = 48;
    glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}
void drawRadioBody()
{
    GLenum mode = GL_QUADS;
    
    //number of vertices
    GLsizei indices = 20;
    glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}

void drawRadioFace()
{
    GLenum mode = GL_QUADS;

    //number of vertices
    GLsizei indices = 4;
    glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}

void drawRadioAntenna()
{
    GLenum mode = GL_QUADS;

    //number of vertices
    GLsizei indices = 24;
    glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}

void drawFloor()
{
    GLenum mode = GL_QUADS;

    //number of vertices
    GLsizei indices = 8;
    glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}

// Create and compile shaders
static GLuint CompileShader(const string& source, GLuint shaderType)
{
    // Create Shader object
    GLuint shaderID = glCreateShader(shaderType);
    const char* src = source.c_str();

    // Attach source code to Shader Object
    glShaderSource(shaderID, 1, &src, nullptr);

    // Compile the Shader
    glCompileShader(shaderID);

    // Return ID of compiled shader
    return shaderID;
}

// Create Program object
static GLuint CreateShaderProgram(const string& vertexShader, const string& fragmentShader)
{
    // Compile Vertex shader
    GLuint vertexShaderComp = CompileShader(vertexShader, GL_VERTEX_SHADER);

    // Compile Fragment shader
    GLuint fragmentShaderComp = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

    // Create Program Object
    GLuint shaderProgram = glCreateProgram();

    // Attach Vertex and Fragment Shaders to program object
    glAttachShader(shaderProgram, vertexShaderComp);
    glAttachShader(shaderProgram, fragmentShaderComp);

    // Link Shaders to create executable program
    glLinkProgram(shaderProgram);

    // Delete compiled shaders
    glDeleteShader(vertexShaderComp);
    glDeleteShader(fragmentShaderComp);

    // Return Shader Program
    return shaderProgram;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1200, 800, "Module Four Assignment", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Set input callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);


    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

    }

    // Vertex Positions  (d, h, w)  // Colors (r,g,b,a)  // Texture (x, y)
    // Start all triangles at the right angle and go counter clockwise
    // If no right angle start at the smallest angle or center point of hex

    GLfloat lampVertices[] = {
        0.0, 0.0,  -3.0,
        0.0, 0.0,  0.0,
        3.0,  0.0,  -3.0,
        3.0,  0.0,  0.0,
    };
    GLubyte lampIndices[] = {
        1,0,2,3,
    };

    GLfloat verticesBottle[] = {

        //faces around hex
        -0.05f,  2.5f, 0.7f,   1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 0   
        -0.05f,  1.7f, 0.7f,   1.0f, 0.6f, 0.4f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 1   
        -0.25f,  1.7f, 0.7f,   1.0f, 0.6f, 0.4f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 2  
        -0.25f,  2.5f, 0.7f,   1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 3  

        -0.05f,  1.7f, 0.3f,   1.0f, 0.6f, 0.4f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 4  
        -0.05f,  2.5f, 0.3f,   1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 5  
        -0.25f,  2.5f, 0.3f,   1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 6 
        -0.25f,  1.7f, 0.3f,   1.0f, 0.6f, 0.4f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 7 

        0.05f,   2.5f, 0.6f,   1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 8  
        0.05f,   1.7f, 0.6f,   1.0f, 0.6f, 0.4f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 9 
        -0.35f,  1.7f, 0.6f,   1.0f, 0.6f, 0.4f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 10 
        -0.35f,  2.5f, 0.6f,   1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 11 

        0.05f,   1.7f, 0.4f,   1.0f, 0.6f, 0.4f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 12  
        0.05f,   2.5f, 0.4f,   1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 13  
        -0.35f,  2.5f, 0.4f,   1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 14 
        -0.35f,  1.7f, 0.4f,   1.0f, 0.6f, 0.4f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 15 

        //central point top  
        -0.15f,  2.5f, 0.5f,   1.0f, 0.6f, 0.4f,   0.0f, 0.0f, // 16
    };
    // labels for position attributes define element indices   20=12
    GLubyte indicesBottle[] = {

         //bottle neck
         0, 1, 3,
         1, 2, 3,
         4, 5, 6,
         4, 6, 7,
         8, 12, 13,
         8, 9, 12,
         11, 15, 14,
         11, 10, 15,
         8, 1, 9,
         8, 0, 1,
         3, 11, 2,
         11, 10, 2,
         14, 6, 15,
         6, 7, 15,
         5, 12, 4,
         5, 13, 12,

         //bottle top hex
         8, 13, 16,
         11, 14, 16,
         0, 3, 16,
         5, 6, 16,
         8, 0, 16,
         3, 11, 16,
         14, 6, 16,
         13, 5, 16,
    };

    // Vertex Positions  (d, h, w)  // Colors (r,g,b,a)  // Texture (x, y)

    GLfloat verticesBottleLeft[]{
        0.2f,   1.7f, 1.0f,   1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 0 
        0.2f,   0.0f, 1.0f,   1.0f, 0.6f, 0.4f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 1   
        -0.5f,  0.0f, 1.0f,   1.0f, 0.6f, 0.4f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 2  
        -0.5f,  1.7f, 1.0f,   1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 3  
    };
    GLubyte indicesBottleLeft[] = {
        3,0,1,2,
    };

    GLfloat verticesBottleFront[]{
         0.2f,  1.7f, 0.0f,    1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 0 
         0.2f,  0.0f, 0.0f,    1.0f, 0.6f, 0.4f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 1   
         0.2f,  0.0f, 1.0f,    1.0f, 0.6f, 0.4f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 2  
         0.2f,  1.7f, 1.0f,    1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 3  

    };
    GLubyte indicesBottleFront[] = {
         2,1,0,3,    
    };

    GLfloat verticesBottleBack[] {
         -0.5f,  1.7f, 0.0f,    1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 0 
         -0.5f,  0.0f, 0.0f,    1.0f, 0.6f, 0.4f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 1   
         -0.5f,  0.0f, 1.0f,    1.0f, 0.6f, 0.4f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 2  
         -0.5f,  1.7f, 1.0f,    1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 3  
    };
    GLubyte indicesBottleBack[] = {
        0,1,2,3,
    };

    GLfloat verticesBottleTop[] = {
         -0.5f,  1.7f, 0.0f,    1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 0 
          0.2f,  1.7f, 0.0f,    1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 1 
          0.2f,  1.7f, 1.0f,    1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 2  
         -0.5f,  1.7f, 1.0f,    1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 3  
         -0.15f, 1.9f, 0.5f,    1.0f, 0.6f, 0.4f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 4
    };
    GLubyte indicesBottleTop[] = {
        0,1,4,
        1,2,4,
        2,3,4,
        3,0,4,
    };
    GLfloat verticesBottleRight[]{
        0.2f,   1.7f, 0.0f,   1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 0 
        0.2f,   0.0f, 0.0f,   1.0f, 0.6f, 0.4f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 1   
        -0.5f,  0.0f, 0.0f,   1.0f, 0.6f, 0.4f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 2  
        -0.5f,  1.7f, 0.0f,   1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 3  
    };
    GLubyte indicesBottleRight[] = {
        3,0,1,2,
    };

    GLfloat verticesRadioBody[]{
        0.0f,     0.0f,    0.0f,      1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 0 
        0.2f,     0.224f,  0.0f,      1.0f, 0.6f, 0.4f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 1   
        -0.247f,  0.624f,  0.0f,      1.0f, 0.6f, 0.4f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 2  
        -0.447f,  0.4f,    0.0f,      1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 3  

        0.0f,     0.0f,    1.5f,      1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 4 
        0.2f,     0.224f,  1.5f,      1.0f, 0.6f, 0.4f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 5   
        -0.247f,  0.624f,  1.5f,      1.0f, 0.6f, 0.4f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 6  
        -0.447f,  0.4f,    1.5f,      1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 7  

    };
    GLubyte indicesRadioBody[] = {
        0,1,2,3,
        4,5,6,7,
        0,4,5,1,
        3,2,6,7,
        0,4,7,3,
    };

    GLfloat verticesRadioFace[]{
        0.2f,     0.224f,  0.0f,      1.0f, 0.6f, 0.4f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 0   
        -0.247f,  0.624f,  0.0f,      1.0f, 0.6f, 0.4f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 1 
        0.2f,     0.224f,  1.5f,      1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 2   
        -0.247f,  0.624f,  1.5f,      1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 3  
    };
    GLubyte indicesRadioFace[] = {
        2,3,1,0,
    };

    GLfloat verticesRadioAntenna[]{
        0.0f,  0.15f,    1.5f,      1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 0 
        0.1f,  0.15f,    1.5f,      1.0f, 0.6f, 0.4f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 1   
        0.0f,  0.25f,    1.5f,      1.0f, 0.6f, 0.4f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 2  
        0.1f,  0.25f,    1.5f,      1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 3  

        0.0f,  0.15f,    2.7f,      1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 4
        0.1f,  0.15f,    2.7f,      1.0f, 0.6f, 0.4f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 5   
        0.0f,  0.25f,    2.7f,      1.0f, 0.6f, 0.4f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 6  
        0.1f,  0.25f,    2.7f,      1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 7 
    };
    GLubyte indicesRadioAntenna[] = {
        0,1,3,2,
        4,5,7,6,
        0,2,6,4,
        3,7,6,2,
        3,1,7,5,
        1,0,4,5,

    };

    GLfloat verticesPaintCan[]{
        //faces around hex
        -0.25f,  1.3f,   0.5f,   1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 0 
         0.25f,  1.3f,   0.5f,   1.0f, 0.6f, 0.4f,   0.5f, 1.0f,   0.0f, 0.0f, 1.0f, // 1   
         0.25f,  0.0f,   0.5f,   1.0f, 0.6f, 0.4f,   0.5f, 0.0f,   0.0f, 0.0f, 1.0f, // 2   
        -0.25f,  0.0f,   0.5f,   1.0f, 0.6f, 0.4f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 3  

         0.25f,  1.3f,  -0.5f,   1.0f, 0.6f, 0.4f,   0.625f, 1.0f,   0.0f, 0.0f, 1.0f, //  4 
        -0.25f,  1.3f,  -0.5f,   1.0f, 0.6f, 0.4f,   0.5, 1.0f,   0.0f, 0.0f, 1.0f, //  5 
        -0.25f,  0.0f,  -0.5f,   1.0f, 0.6f, 0.4f,   0.5f, 0.0f,   0.0f, 0.0f, 1.0f, //  6
         0.25f,  0.0f,  -0.5f,   1.0f, 0.6f, 0.4f,   0.625f, 0.0f,   0.0f, 0.0f, 1.0f, //  7 

        0.5f,   1.3f,   0.25f,   1.0f, 0.6f, 0.4f,   0.5f, 1.0f,   0.0f, 0.0f, 1.0f, //  8  
        0.5f,   1.3f,  -0.25f,   1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, //  9
        0.5f,   0.0f,  -0.25f,   1.0f, 0.6f, 0.4f,   1.0, 0.0f,   0.0f, 0.0f, 1.0f, //  10
        0.5f,   0.0f,   0.25f,   1.0f, 0.6f, 0.4f,   0.5f, 0.0f,   0.0f, 0.0f, 1.0f, //  11

       -0.5f,   1.3f,  -0.25f,   1.0f, 0.6f, 0.4f,   0.875f, 1.0f,   0.0f, 0.0f, 1.0f, //  12 
       -0.5f,   1.3f,  0.25f,    1.0f, 0.6f, 0.4f,   0.75f, 1.0f,   0.0f, 0.0f, 1.0f, //  13 
       -0.5f,   0.0f,  0.25f,    1.0f, 0.6f, 0.4f,   0.75f, 0.0f,   0.0f, 0.0f, 1.0f, //  14
       -0.5f,   0.0f,  -0.25f,   1.0f, 0.6f, 0.4f,   0.875f, 0.0f,   0.0f, 0.0f, 1.0f, //  15 
    };
    GLubyte indicesPaintCan[] = {
        //sides
        0,3,2,1,
        1,2,11,8,
        8,11,10,9,
        9,10,7,4,
        4,7,6,5,
        5,6,15,12,
        12,15,14,13,
        13,14,3,0,

    };

    GLfloat verticesPaintCanTop[]{
        //faces around hex
        -0.25f,  1.3f,   0.5f,   1.0f, 0.6f, 0.4f,   0.3f, 1.0f,   0.0f, 0.0f, 1.0f, // 0 
         0.25f,  1.3f,   0.5f,   1.0f, 0.6f, 0.4f,   0.6f, 1.0f,   0.0f, 0.0f, 1.0f, // 1   
         0.25f,  0.0f,   0.5f,   1.0f, 0.6f, 0.4f,   0.6f, 0.0f,   0.0f, 0.0f, 1.0f, // 2   
        -0.25f,  0.0f,   0.5f,   1.0f, 0.6f, 0.4f,   0.3f, 0.0f,   0.0f, 0.0f, 1.0f, // 3  

         0.25f,  1.3f,  -0.5f,   1.0f, 0.6f, 0.4f,   0.3f, 1.0f,   0.0f, 0.0f, 1.0f, //  4 
        -0.25f,  1.3f,  -0.5f,   1.0f, 0.6f, 0.4f,   0.6f, 1.0f,   0.0f, 0.0f, 1.0f, //  5 
        -0.25f,  0.0f,  -0.5f,   1.0f, 0.6f, 0.4f,   0.6f, 0.0f,   0.0f, 0.0f, 1.0f, //  6
         0.25f,  0.0f,  -0.5f,   1.0f, 0.6f, 0.4f,   0.3f, 0.0f,   0.0f, 0.0f, 1.0f, //  7 

        0.5f,   1.3f,   0.25f,   1.0f, 0.6f, 0.4f,   0.3f, 1.0f,   0.0f, 0.0f, 1.0f, //  8  
        0.5f,   1.3f,  -0.25f,   1.0f, 0.6f, 0.4f,   0.6f, 1.0f,   0.0f, 0.0f, 1.0f, //  9
        0.5f,   0.0f,  -0.25f,   1.0f, 0.6f, 0.4f,   0.6f, 0.0f,   0.0f, 0.0f, 1.0f, //  10
        0.5f,   0.0f,   0.25f,   1.0f, 0.6f, 0.4f,   0.3f, 0.0f,   0.0f, 0.0f, 1.0f, //  11

       -0.5f,   1.3f,  -0.25f,   1.0f, 0.6f, 0.4f,   0.3f, 1.0f,   0.0f, 0.0f, 1.0f, //  12 
       -0.5f,   1.3f,  0.25f,    1.0f, 0.6f, 0.4f,   0.6f, 1.0f,   0.0f, 0.0f, 1.0f, //  13 
       -0.5f,   0.0f,  0.25f,    1.0f, 0.6f, 0.4f,   0.6f, 0.0f,   0.0f, 0.0f, 1.0f, //  14
       -0.5f,   0.0f,  -0.25f,   1.0f, 0.6f, 0.4f,   0.3f, 0.0f,   0.0f, 0.0f, 1.0f, //  15 

    //central points 
        0.0f,  1.3f, 0.0f,   1.0f, 0.6f, 0.4f,   0.5f, 0.0f,   0.0f, 0.0f, 1.0f,// 16
        0.0f,  0.0f, 0.0f,   1.0f, 0.6f, 0.4f,   0.5f, 0.0f,   0.0f, 0.0f, 1.0f,// 17
    };
    GLubyte indicesPaintCanTop[] = {
        //top
        1,0,16,
        8,1,16,
        9,8,16,
        4,9,16,
        5,4,16,
        12,5,16,
        13,12,16,
        0,13,16,

        //bottom
        3,2,17,
        2,11,17,
        11,10,17,
        10,7,17,
        7,6,17,
        6,15,17,
        15,14,17,
        14,3,17,
    };

    GLfloat verticesFloor[] = {
        //3d plane for scene
        -3.5f,  0.0f, -3.5f,    1.0f, 0.6f, 0.4f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,// 0
        -3.5f,  0.0f, 3.5f,     1.0f, 0.6f, 0.4f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,// 1
        3.5f,   0.0f, -3.5f,    1.0f, 0.6f, 0.4f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,// 2
        3.5f,   0.0f, 3.5f,     1.0f, 0.6f, 0.4f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,// 3

        -3.5f,   0.0f, 3.5f,    1.0f, 0.6f, 0.4f,    0.0f, 0.0f,   0.0f, 0.0f, 1.0f,// 4
        3.5f,   0.0f, 3.5f,     1.0f, 0.6f, 0.4f,    1.0f, 0.0f,   0.0f, 0.0f, 1.0f,// 5
        -3.5f,   0.0f, 7.0f,    1.0f, 0.6f, 0.4f,    0.0f, 0.5f,   0.0f, 0.0f, 1.0f,// 6
        3.5f,   0.0f, 7.0f,     1.0f, 0.6f, 0.4f,    1.0f, 0.5f,   0.0f, 0.0f, 1.0f,// 7

    };
    GLubyte indicesFloor [] = {
        //floor plane
        0,2,3,1,
        5,4,6,7,
    };

    // Plane positions
    glm::vec3 planePositions[] =
    {
        glm::vec3(0.0f, 8.0f, 0.0f),   // light source translation
        glm::vec3(.5f, 0.0f, 1.0f),  // bottle pieces translation
        glm::vec3(0.8f, 0.0f, 2.7f),     // paint can translation
        glm::vec3(-1.2f, 0.0f, 0.6f),  // floor translation
        glm::vec3(1.8f, 0.0f, 0.7f),   // radio pieces translation
    };


    // Plane rotations
    glm::float32 planeXRotation[] =
    {
        0.0f, 0.0f, 0.0f, 0.0f, // light rotation
    };


    // labels for position attributes define element indices
    // Enable Depth Buffer
    glEnable(GL_DEPTH_TEST);

    // Wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Create all VBOs
    GLuint lampVBO, lampEBO, lampVAO;
    GLuint floorVBO, floorEBO, floorVAO;
    GLuint bottleVBO, bottleEBO, bottleVAO;
    GLuint bottleLeftVBO, bottleLeftEBO, bottleLeftVAO;
    GLuint bottleFrontVBO, bottleFrontEBO, bottleFrontVAO;
    GLuint bottleBackVBO, bottleBackEBO, bottleBackVAO;
    GLuint bottleRightVBO, bottleRightEBO, bottleRightVAO;
    GLuint bottleTopVBO, bottleTopEBO, bottleTopVAO;
    GLuint paintCanVBO, paintCanEBO, paintCanVAO;
    GLuint paintCanTopVBO, paintCanTopEBO, paintCanTopVAO;
    GLuint radioBodyVBO, radioBodyEBO, radioBodyVAO;
    GLuint radioFaceVBO, radioFaceEBO, radioFaceVAO;
    GLuint radioAntennaVBO, radioAntennaEBO, radioAntennaVAO;


    //LAMP CREATION AND BINDING
    glGenBuffers(1, &lampVBO);
        glGenBuffers(1, &lampEBO);
        glGenVertexArrays(1, &lampVAO); // Create VAO
        glBindVertexArray(lampVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lampVBO); // Select VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lampEBO); // Select EBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(lampVertices), lampVertices, GL_STATIC_DRAW); // Load vertex attributes
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lampIndices), lampIndices, GL_STATIC_DRAW); // Load indices
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)0);  // Specify attribute location and layout to GPU (index, size, type, normalized, stride, ptr (offset))
        glEnableVertexAttribArray(0);  //position attribute(x y z) is at location 0 and (r g b a) is at location 1
    glBindVertexArray(0); //deactivate VAO

    //FLOOR CREATION AND BINDING
    glGenBuffers(1, &floorVBO); // Create VBO
        glGenBuffers(1, &floorEBO); // create EBO
        glGenVertexArrays(1, &floorVAO); // Create VAO
        glBindVertexArray(floorVAO);
        glBindBuffer(GL_ARRAY_BUFFER, floorVBO); // Select VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO); // Select EBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesFloor), verticesFloor, GL_STATIC_DRAW); // Load vertex attributes
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesFloor), indicesFloor, GL_STATIC_DRAW); // Load indices
        // Specify attribute location and layout to GPU (index, size, type, normalized, stride, ptr (offset))
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
        //position attribute(x y z) is at location 0 and (r g b a) is at location 1
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    //BOTTLE CREATION AND BINDING
    glGenBuffers(1, &bottleVBO); // Create VBO
        glGenBuffers(1, &bottleEBO); // create EBO
        glGenVertexArrays(1, &bottleVAO); // Create VAO
        glBindVertexArray(bottleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, bottleVBO); // Select VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottleEBO); // Select EBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBottle), verticesBottle, GL_STATIC_DRAW); // Load vertex attributes
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesBottle), indicesBottle, GL_STATIC_DRAW); // Load indices
        // Specify attribute location and layout to GPU (index, size, type, normalized, stride, ptr (offset))
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
        //position attribute(x y z) is at location 0 and (r g b a) is at location 1
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    glBindVertexArray(0); //deactivate VAO

    //BOTTLE LEFT CREATION AND BINDING
    glGenBuffers(1, &bottleLeftVBO); // Create VBO
        glGenBuffers(1, &bottleLeftEBO); // create EBO
        glGenVertexArrays(1, &bottleLeftVAO); // Create VAO
        glBindVertexArray(bottleLeftVAO);
        glBindBuffer(GL_ARRAY_BUFFER, bottleLeftVBO); // Select VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottleLeftEBO); // Select EBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBottleLeft), verticesBottleLeft, GL_STATIC_DRAW); // Load vertex attributes
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesBottleLeft), indicesBottleLeft, GL_STATIC_DRAW); // Load indices
        // Specify attribute location and layout to GPU (index, size, type, normalized, stride, ptr (offset))
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
        //position attribute(x y z) is at location 0 and (r g b a) is at location 1
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    glBindVertexArray(0); //deactivate VAO

        //BOTTLE LEFT CREATION AND BINDING
    glGenBuffers(1, &bottleFrontVBO); // Create VBO
        glGenBuffers(1, &bottleFrontEBO); // create EBO
        glGenVertexArrays(1, &bottleFrontVAO); // Create VAO
        glBindVertexArray(bottleFrontVAO);
        glBindBuffer(GL_ARRAY_BUFFER, bottleFrontVBO); // Select VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottleFrontEBO); // Select EBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBottleFront), verticesBottleFront, GL_STATIC_DRAW); // Load vertex attributes
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesBottleFront), indicesBottleFront, GL_STATIC_DRAW); // Load indices
        // Specify attribute location and layout to GPU (index, size, type, normalized, stride, ptr (offset))
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
        //position attribute(x y z) is at location 0 and (r g b a) is at location 1
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    glBindVertexArray(0); //deactivate VAO

            //BOTTLE LEFT CREATION AND BINDING
    glGenBuffers(1, &bottleBackVBO); // Create VBO
        glGenBuffers(1, &bottleBackEBO); // create EBO
        glGenVertexArrays(1, &bottleBackVAO); // Create VAO
        glBindVertexArray(bottleBackVAO);
        glBindBuffer(GL_ARRAY_BUFFER, bottleBackVBO); // Select VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottleBackEBO); // Select EBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBottleBack), verticesBottleBack, GL_STATIC_DRAW); // Load vertex attributes
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesBottleBack), indicesBottleBack, GL_STATIC_DRAW); // Load indices
        // Specify attribute location and layout to GPU (index, size, type, normalized, stride, ptr (offset))
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
        //position attribute(x y z) is at location 0 and (r g b a) is at location 1
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    glBindVertexArray(0); //deactivate VAO

            //BOTTLE LEFT CREATION AND BINDING
    glGenBuffers(1, &bottleRightVBO); // Create VBO
        glGenBuffers(1, &bottleRightEBO); // create EBO
        glGenVertexArrays(1, &bottleRightVAO); // Create VAO
        glBindVertexArray(bottleRightVAO);
        glBindBuffer(GL_ARRAY_BUFFER, bottleRightVBO); // Select VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottleRightEBO); // Select EBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBottleRight), verticesBottleRight, GL_STATIC_DRAW); // Load vertex attributes
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesBottleRight), indicesBottleRight, GL_STATIC_DRAW); // Load indices
        // Specify attribute location and layout to GPU (index, size, type, normalized, stride, ptr (offset))
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
        //position attribute(x y z) is at location 0 and (r g b a) is at location 1
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    glBindVertexArray(0); //deactivate VAO

    glGenBuffers(1, &bottleTopVBO); // Create VBO
        glGenBuffers(1, &bottleTopEBO); // create EBO
        glGenVertexArrays(1, &bottleTopVAO); // Create VAO
        glBindVertexArray(bottleTopVAO);
        glBindBuffer(GL_ARRAY_BUFFER, bottleTopVBO); // Select VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottleTopEBO); // Select EBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBottleTop), verticesBottleTop, GL_STATIC_DRAW); // Load vertex attributes
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesBottleTop), indicesBottleTop, GL_STATIC_DRAW); // Load indices
        // Specify attribute location and layout to GPU (index, size, type, normalized, stride, ptr (offset))
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
        //position attribute(x y z) is at location 0 and (r g b a) is at location 1
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    glBindVertexArray(0); //deactivate VAO

    glGenBuffers(1, &paintCanVBO); // Create VBO
        glGenBuffers(1, &paintCanEBO); // create EBO
        glGenVertexArrays(1, &paintCanVAO); // Create VAO
        glBindVertexArray(paintCanVAO);
        glBindBuffer(GL_ARRAY_BUFFER, paintCanVBO); // Select VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, paintCanEBO); // Select EBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPaintCan), verticesPaintCan, GL_STATIC_DRAW); // Load vertex attributes
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPaintCan), indicesPaintCan, GL_STATIC_DRAW); // Load indices
        // Specify attribute location and layout to GPU (index, size, type, normalized, stride, ptr (offset))
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
        //position attribute(x y z) is at location 0 and (r g b a) is at location 1
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    glBindVertexArray(0); //deactivate VAO

    glGenBuffers(1, &paintCanTopVBO); // Create VBO
        glGenBuffers(1, &paintCanTopEBO); // create EBO
        glGenVertexArrays(1, &paintCanTopVAO); // Create VAO
        glBindVertexArray(paintCanTopVAO);
        glBindBuffer(GL_ARRAY_BUFFER, paintCanTopVBO); // Select VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, paintCanTopEBO); // Select EBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPaintCanTop), verticesPaintCanTop, GL_STATIC_DRAW); // Load vertex attributes
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPaintCanTop), indicesPaintCanTop, GL_STATIC_DRAW); // Load indices
        // Specify attribute location and layout to GPU (index, size, type, normalized, stride, ptr (offset))
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
        //position attribute(x y z) is at location 0 and (r g b a) is at location 1
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    glBindVertexArray(0); //deactivate VAO

    glGenBuffers(1, &radioBodyVBO); // Create VBO
        glGenBuffers(1, &radioBodyEBO); // create EBO
        glGenVertexArrays(1, &radioBodyVAO); // Create VAO
        glBindVertexArray(radioBodyVAO);
        glBindBuffer(GL_ARRAY_BUFFER, radioBodyVBO); // Select VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, radioBodyEBO); // Select EBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRadioBody), verticesRadioBody, GL_STATIC_DRAW); // Load vertex attributes
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesRadioBody), indicesRadioBody, GL_STATIC_DRAW); // Load indices
        // Specify attribute location and layout to GPU (index, size, type, normalized, stride, ptr (offset))
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
        //position attribute(x y z) is at location 0 and (r g b a) is at location 1
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    glBindVertexArray(0); //deactivate VAO

    glGenBuffers(1, &radioFaceVBO); // Create VBO
        glGenBuffers(1, &radioFaceEBO); // create EBO
        glGenVertexArrays(1, &radioFaceVAO); // Create VAO
        glBindVertexArray(radioFaceVAO);
        glBindBuffer(GL_ARRAY_BUFFER, radioFaceVBO); // Select VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, radioFaceEBO); // Select EBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRadioFace), verticesRadioFace, GL_STATIC_DRAW); // Load vertex attributes
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesRadioFace), indicesRadioFace, GL_STATIC_DRAW); // Load indices
        // Specify attribute location and layout to GPU (index, size, type, normalized, stride, ptr (offset))
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
        //position attribute(x y z) is at location 0 and (r g b a) is at location 1
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    glBindVertexArray(0); //deactivate VAO

    glGenBuffers(1, &radioAntennaVBO); // Create VBO
        glGenBuffers(1, &radioAntennaEBO); // create EBO
        glGenVertexArrays(1, &radioAntennaVAO); // Create VAO
        glBindVertexArray(radioAntennaVAO);
        glBindBuffer(GL_ARRAY_BUFFER, radioAntennaVBO); // Select VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, radioAntennaEBO); // Select EBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRadioAntenna), verticesRadioAntenna, GL_STATIC_DRAW); // Load vertex attributes
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesRadioAntenna), indicesRadioAntenna, GL_STATIC_DRAW); // Load indices
        // Specify attribute location and layout to GPU (index, size, type, normalized, stride, ptr (offset))
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
        //position attribute(x y z) is at location 0 and (r g b a) is at location 1
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    glBindVertexArray(0); //deactivate VAO

     // Load textures
    int floorTexWidth, floorTexHeight;
    int bottleTexWidth, bottleTexHeight;
    int bottleLeftTexWidth, bottleLeftTexHeight;
    int bottleFrontTexWidth, bottleFrontTexHeight;
    int bottleBackTexWidth, bottleBackTexHeight;
    int bottleRightTexWidth, bottleRightTexHeight;
    int paintCanTexWidth, paintCanTexHeight;
    int paintCanTopTexWidth, paintCanTopTexHeight;
    int radioBodyTexWidth, radioBodyTexHeight;
    int radioFaceTexWidth, radioFaceTexHeight;
    int radioAntennaTexWidth, radioAntennaTexHeight;

    unsigned char* floorImage = SOIL_load_image("floor.png", &floorTexWidth, &floorTexHeight, 0, SOIL_LOAD_RGB);
    unsigned char* bottleImage = SOIL_load_image("bottleNeck.png", &bottleTexWidth, &bottleTexHeight, 0, SOIL_LOAD_RGB);
    unsigned char* bottleLeftImage = SOIL_load_image("bottleLeft.png", &bottleLeftTexWidth, &bottleLeftTexHeight, 0, SOIL_LOAD_RGB);
    unsigned char* bottleFrontImage = SOIL_load_image("bottleFront.png", &bottleFrontTexWidth, &bottleFrontTexHeight, 0, SOIL_LOAD_RGB);
    unsigned char* bottleBackImage = SOIL_load_image("bottleBack.png", &bottleBackTexWidth, &bottleBackTexHeight, 0, SOIL_LOAD_RGB);
    unsigned char* bottleRightImage = SOIL_load_image("bottleRight.png", &bottleRightTexWidth, &bottleRightTexHeight, 0, SOIL_LOAD_RGB);
    unsigned char* paintCanImage = SOIL_load_image("paintCan.png", &paintCanTexWidth, &paintCanTexHeight, 0, SOIL_LOAD_RGB);
    unsigned char* paintCanTopImage = SOIL_load_image("paintCanTop.png", &paintCanTopTexWidth, &paintCanTopTexHeight, 0, SOIL_LOAD_RGB);
    unsigned char* radioBodyImage = SOIL_load_image("radioBody.png", &radioBodyTexWidth, &radioBodyTexHeight, 0, SOIL_LOAD_RGB);
    unsigned char* radioFaceImage = SOIL_load_image("radio.png", &radioFaceTexWidth, &radioFaceTexHeight, 0, SOIL_LOAD_RGB);
    unsigned char* radioAntennaImage = SOIL_load_image("antenna.png", &radioAntennaTexWidth, &radioAntennaTexHeight, 0, SOIL_LOAD_RGB);


    // Genreate Textures
    GLuint floorTexture;
        glGenTextures(1, &floorTexture);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, floorTexWidth, floorTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, floorImage);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(floorImage);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint bottleTexture;
        glGenTextures(1, &bottleTexture);
        glBindTexture(GL_TEXTURE_2D, bottleTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bottleTexWidth, bottleTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bottleImage);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(bottleImage);
    glBindTexture(GL_TEXTURE_2D, 1);

    GLuint bottleLeftTexture;
        glGenTextures(1, &bottleLeftTexture);
        glBindTexture(GL_TEXTURE_2D, bottleLeftTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bottleLeftTexWidth, bottleLeftTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bottleLeftImage);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(bottleLeftImage);
    glBindTexture(GL_TEXTURE_2D, 1);

    GLuint bottleFrontTexture;
        glGenTextures(1, &bottleFrontTexture);
        glBindTexture(GL_TEXTURE_2D, bottleFrontTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bottleFrontTexWidth, bottleFrontTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bottleFrontImage);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(bottleFrontImage);
    glBindTexture(GL_TEXTURE_2D, 1);

    GLuint bottleBackTexture;
        glGenTextures(1, &bottleBackTexture);
        glBindTexture(GL_TEXTURE_2D, bottleBackTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bottleBackTexWidth, bottleBackTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bottleBackImage);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(bottleBackImage);
    glBindTexture(GL_TEXTURE_2D, 1);

    GLuint bottleRightTexture;
    glGenTextures(1, &bottleRightTexture);
        glBindTexture(GL_TEXTURE_2D, bottleRightTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bottleRightTexWidth, bottleRightTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bottleRightImage);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(bottleRightImage);
    glBindTexture(GL_TEXTURE_2D, 1);

    GLuint paintCanTexture;
        glGenTextures(1, &paintCanTexture);
        glBindTexture(GL_TEXTURE_2D, paintCanTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, paintCanTexWidth, paintCanTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, paintCanImage);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(paintCanImage);
    glBindTexture(GL_TEXTURE_2D, 1);

    GLuint paintCanTopTexture;
        glGenTextures(1, &paintCanTopTexture);
        glBindTexture(GL_TEXTURE_2D, paintCanTopTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, paintCanTopTexWidth, paintCanTopTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, paintCanTopImage);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(paintCanTopImage);
    glBindTexture(GL_TEXTURE_2D, 1);

    GLuint radioBodyTexture;
        glGenTextures(1, &radioBodyTexture);
        glBindTexture(GL_TEXTURE_2D, radioBodyTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, radioBodyTexWidth, radioBodyTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, radioBodyImage);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(radioBodyImage);
    glBindTexture(GL_TEXTURE_2D, 1);

    GLuint radioFaceTexture;
        glGenTextures(1, &radioFaceTexture);
        glBindTexture(GL_TEXTURE_2D, radioFaceTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, radioFaceTexWidth, radioFaceTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, radioFaceImage);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(radioFaceImage);
    glBindTexture(GL_TEXTURE_2D, 1);

    GLuint radioAntennaTexture;
        glGenTextures(1, &radioAntennaTexture);
        glBindTexture(GL_TEXTURE_2D, radioAntennaTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, radioAntennaTexWidth, radioAntennaTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, radioAntennaImage);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(radioAntennaImage);
    glBindTexture(GL_TEXTURE_2D, 1);

    // Vertex shader source code
    string vertexShaderSource =
        "#version 330 core\n"
        "layout(location = 0) in vec3 vPosition;"
        "layout(location = 1) in vec3 aColor;"
        "layout(location = 2) in vec2 texCoord;"
        "layout(location = 3) in vec3 normal;"
        "out vec3 oColor;"
        "out vec2 oTexCoord;"
        "out vec3 oNormal;"
        "out vec3 FragPos;"
        "uniform mat4 model;"
        "uniform mat4 view;"
        "uniform mat4 projection;"
        "void main()\n"
        "{\n"
        "gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
        "oColor = aColor;"
        "oTexCoord = texCoord;"
        "oNormal = mat3(transpose(inverse(model))) * normal;"
        "FragPos = vec3(model * vec4(vPosition, 1.0f));"
        "}\n";

    // Vertex Fragment shader source code
    string fragmentShaderSource =
        "#version 330 core\n"
        "in vec3 oColor;"
        "in vec2 oTexCoord;"
        "in vec3 oNormal;"
        "in vec3 FragPos;"
        "out vec4 fragColor;"
        "uniform sampler2D myTexture;"
        "uniform vec3 objectColor;"
        "uniform vec3 lightColor;"
        "uniform vec3 lightPos;"
        "uniform vec3 viewPos;"
        "void main()\n"
        "{\n"
        "//Ambient\n"
        "float ambientStrength = 2.5f;"
        "vec3 ambient = ambientStrength * lightColor;"
        "//Diffuse\n"
        "vec3 norm = normalize(oNormal);"
        "vec3 lightDir = normalize(lightPos - FragPos);"
        "float diff = max(dot(norm, lightDir), 0.0);"
        "vec3 diffuse = diff * lightColor;"
        "//Specularity\n"
        "float specularStrength = 2.0f;"
        "vec3 viewDir = normalize(viewPos - FragPos);"
        "vec3 reflectDir = reflect(-lightDir, norm);"
        "float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);"
        "vec3 specular = specularStrength * spec * lightColor;"
        "vec3 result = (ambient + diffuse + specular) * objectColor;"
        "fragColor = texture(myTexture, oTexCoord) * vec4(result, 1.0f);"
        "}\n";

    string lampVertexShaderSource =
        "#version 330 core\n"
        "layout(location = 0) in vec3 vPosition;"
        "uniform mat4 model;"
        "uniform mat4 view;"
        "uniform mat4 projection;"
        "void main()\n"
        "{\n"
        "gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
        "}\n";

    string lampFragmentShaderSource =
        "#version 330 core\n"
        "out vec4 fragColor;"
        "void main()\n"
        "{\n"
        "fragColor = vec4(2.0f);"
        "}\n";

    // Create Shader Program
    GLuint fullShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
    GLuint lampShaderProgram = CreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource);


    // Use Shader Program exe
    glUseProgram(fullShaderProgram);

    // Declare matrices
    glm::mat4 projectionMatrix;

    // transform projection perspective(field of view, width and height as floats, near, far)
    projectionMatrix = glm::perspective(fov, (GLfloat)width / (GLfloat)height, 0.1f, 100.f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // Set delta time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Resize window and graphics simultaneously
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        // switch between 2D and 3D
        if (keys[GLFW_KEY_O])
            projectionMatrix = glm::ortho(-5.0, 5.0, -5.0, 5.0, -5.0, 5.0);
        else if (keys[GLFW_KEY_P])
            projectionMatrix = glm::perspective(fov, (GLfloat)width / (GLfloat)height, 0.1f, 100.f);


        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Call shader per-frame when updating attribs
        glUseProgram(fullShaderProgram);
        

        // transform camera
        viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFrontBack, cameraUpDown);

        // Select uniform shader and variable and send to shader
        GLuint modelLoc = glGetUniformLocation(fullShaderProgram, "model");
        GLuint viewLoc = glGetUniformLocation(fullShaderProgram, "view");
        GLuint projectionLoc = glGetUniformLocation(fullShaderProgram, "projection");
        
        // Light 1
        GLint objectColorLoc = glGetUniformLocation(fullShaderProgram, "objectColor"); // Get light location
        GLint lightColorLoc = glGetUniformLocation(fullShaderProgram, "lightColor"); // Get object color location
        GLint lightPosLoc = glGetUniformLocation(fullShaderProgram, "lightPos"); // Get light position
        GLint viewPosLoc = glGetUniformLocation(fullShaderProgram, "viewPos"); // Get light position

        // Pass transform to Shader
        glUniform3f(objectColorLoc, 0.39f, 0.39f, 0.39f); //Assign color of object
        glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // Color of light
        glUniform3f(lightPosLoc, lightPosition.x, lightPosition.y, lightPosition.z); // Set light position
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        // draw front of bottle and add texture
        glActiveTexture(GL_TEXTURE);
            glBindTexture(GL_TEXTURE_2D, floorTexture);
            glBindVertexArray(floorVAO);
            glm::mat4 modelMatrixFloor(1.0f);
            modelMatrixFloor = glm::translate(modelMatrixFloor, planePositions[3]);
            modelMatrixFloor = glm::rotate(modelMatrixFloor, 35.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate Primative
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrixFloor));
            drawFloor();
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE);
            glBindTexture(GL_TEXTURE_2D, bottleLeftTexture);
            glBindVertexArray(bottleLeftVAO);
            glm::mat4 modelMatrixBottleLeft(1.0f);
            modelMatrixBottleLeft = glm::translate(modelMatrixBottleLeft, planePositions[1]);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrixBottleLeft));
            drawBottleLeft();
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE);
            glBindTexture(GL_TEXTURE_2D, bottleFrontTexture);
            glBindVertexArray(bottleFrontVAO);
            glm::mat4 modelMatrixBottleFront(1.0f);
            modelMatrixBottleFront = glm::translate(modelMatrixBottleFront, planePositions[1]);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrixBottleFront));
            drawBottleFront();
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE);
            glBindTexture(GL_TEXTURE_2D, bottleBackTexture);
            glBindVertexArray(bottleBackVAO);
            glm::mat4 modelMatrixBottleBack(1.0f);
            modelMatrixBottleBack = glm::translate(modelMatrixBottleBack, planePositions[1]);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrixBottleBack));
            drawBottleBack();
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE);
            glBindTexture(GL_TEXTURE_2D, bottleRightTexture);
            glBindVertexArray(bottleRightVAO);
            glm::mat4 modelMatrixBottleRight(1.0f);
            modelMatrixBottleRight = glm::translate(modelMatrixBottleRight, planePositions[1]);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrixBottleRight));
            drawBottleRight();
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE);
            glBindTexture(GL_TEXTURE_2D, bottleBackTexture);
            glBindVertexArray(bottleTopVAO);
            glm::mat4 modelMatrixBottleTop(1.0f);
            modelMatrixBottleTop = glm::translate(modelMatrixBottleTop, planePositions[1]);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrixBottleTop));
            drawBottleTop();
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE);
            glBindTexture(GL_TEXTURE_2D, bottleTexture);
            glBindVertexArray(bottleVAO);
            glm::mat4 modelMatrixBottle(1.0f);
            modelMatrixBottle = glm::translate(modelMatrixBottle, planePositions[1]);
           
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrixBottle));
            drawBottle();
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE);
            glBindTexture(GL_TEXTURE_2D, paintCanTexture);
            glBindVertexArray(paintCanVAO);
            glm::mat4 modelMatrixPaintCan(1.0f);
            modelMatrixPaintCan = glm::translate(modelMatrixPaintCan, planePositions[2]);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrixPaintCan));
            drawPaintCan();
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE);
            glBindTexture(GL_TEXTURE_2D, paintCanTopTexture);
            glBindVertexArray(paintCanTopVAO);
            glm::mat4 modelMatrixPaintCanTop(1.0f);
            modelMatrixPaintCanTop = glm::translate(modelMatrixPaintCanTop, planePositions[2]);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrixPaintCanTop));
            drawPaintCanTop();
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE);
            glBindTexture(GL_TEXTURE_2D, radioBodyTexture);
            glBindVertexArray(radioBodyVAO);
            glm::mat4 modelMatrixRadioBody(1.0f);
            modelMatrixRadioBody = glm::translate(modelMatrixRadioBody, planePositions[4]);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrixRadioBody));
            drawRadioBody();
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE);
            glBindTexture(GL_TEXTURE_2D, radioFaceTexture);
            glBindVertexArray(radioFaceVAO);
            glm::mat4 modelMatrixRadioFace(1.0f);
            modelMatrixRadioFace = glm::translate(modelMatrixRadioFace, planePositions[4]);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrixRadioFace));
            drawRadioFace();
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE);
            glBindTexture(GL_TEXTURE_2D, radioAntennaTexture);
            glBindVertexArray(radioAntennaVAO);
            glm::mat4 modelMatrixRadioAntenna(1.0f);
            modelMatrixRadioAntenna = glm::translate(modelMatrixRadioAntenna, planePositions[4]);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrixRadioAntenna));
            drawRadioAntenna();
            glBindVertexArray(0);
        glUseProgram(0); //close shader to use new one for light

        glUseProgram(lampShaderProgram);
            //first lamp
            GLuint LampModelLoc = glGetUniformLocation(lampShaderProgram, "model");
            GLuint lampViewLoc = glGetUniformLocation(lampShaderProgram, "view");
            GLuint lampProjectionLoc = glGetUniformLocation(lampShaderProgram, "projection");
            glUniformMatrix4fv(lampViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
            glUniformMatrix4fv(lampProjectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
            glBindVertexArray(lampVAO); // Activate
            glm::mat4 modelMatrix1(1.0f);
            modelMatrix1 = glm::translate(modelMatrix1, planePositions[0] * lightPosition);
            modelMatrix1 = glm::rotate(modelMatrix1, planeXRotation[0] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate Primative
            modelMatrix1 = glm::scale(modelMatrix1, glm::vec3(0.3f, 0.3f, 0.3f)); // Scale Primative
            glUniformMatrix4fv(LampModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix1));
            draw(); // Draw primitave
            glBindVertexArray(0);
        glUseProgram(0);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events 
        glfwPollEvents();

        process_input(window);
        TransformCamera();
    }

    glfwTerminate();
    return 0;
}

// Define Input callback functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Keyboard key action controls
    if (action == GLFW_PRESS)
        keys[key] = true;
    else if (action == GLFW_RELEASE)
        keys[key] = false;
}

// Define Scroll callback funcion
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Set incriments for camera speed scroll
    cameraSpeed -= yoffset * 0.001f;

    // Set camera speed limits
    if (cameraSpeed < 0.001f)
        cameraSpeed = 0.001f;
    if (cameraSpeed > 0.05f)
        cameraSpeed = 0.05f;
}

//Define Cursor callback function
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    
   /*if (firstMouseMove)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouseMove = false;
    }*/

    // Calculate cursor offset
    xChange = xpos - lastX;
    yChange = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if (isOrbiting)
    {
        yaw -= xChange;
        pitch += yChange;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        // Azimuth Altitude formula
        cameraDirection.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
        cameraDirection.y = sinf(glm::radians(pitch));
        cameraDirection.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));
        cameraFrontBack = glm::normalize(cameraDirection);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Mouse button action controls
    if (action == GLFW_PRESS)
        mouseButtons[button] = true;
    else if (action == GLFW_RELEASE)
        mouseButtons[button] = false;
}

void process_input(GLFWwindow* window)
{
    // Set speed of camera movement
    float cameraOffset = cameraSpeed * deltaTime;

    // Set Esc button to close window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Set Camera movement controls 

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition += cameraSpeed * cameraFrontBack;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition -= cameraSpeed * cameraFrontBack;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition -= glm::normalize(glm::cross(cameraFrontBack, cameraUpDown)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition += glm::normalize(glm::cross(cameraFrontBack, cameraUpDown)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPosition += cameraSpeed * cameraUpDown;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPosition -= cameraSpeed * cameraUpDown;
}


// Define function to activate mouse orbiting
void TransformCamera()
{
    if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT])
        isOrbiting = true;
    else
        isOrbiting = false;
}
