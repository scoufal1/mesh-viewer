// Bryn Mawr College, alinen, 2020
//

#include "AGL.h"
#include "AGLM.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include "mesh.h"
#include "osutils.h"

using namespace std;
using namespace glm;
using namespace agl;

// globals
Mesh theModel;
int theCurrentModel = 0;
vector<string> theModelNames;

float lastXPos = 500.0f/2;
float lastYPos = 500.0f/2;
float azimuth = 0.0f;
float elevation = 0.0f;
float dist = 2.5f;

// camera
glm::mat4 camera = glm::lookAt(glm::vec3(0,0,dist), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

bool mouse_pressed = false;
bool shift_pressed = false;

// diffuse color
glm::vec3 diffuse_color(0.4f, 0.6f, 1.0f);

// OpenGL IDs
GLuint theVboPosId;
GLuint theVboNormalId;
GLuint theElementbuffer;

static void LoadModel()
{
   theModel.loadPLY("../original-models/elephant.ply");

   glBindBuffer(GL_ARRAY_BUFFER, theVboPosId);
   glBufferData(GL_ARRAY_BUFFER, theModel.numVertices() * 3 * sizeof(float), theModel.positions(), GL_DYNAMIC_DRAW);

   glBindBuffer(GL_ARRAY_BUFFER, theVboNormalId);
   glBufferData(GL_ARRAY_BUFFER, theModel.numVertices() * 3 * sizeof(float), theModel.normals(), GL_DYNAMIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theElementbuffer);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, theModel.numTriangles() * 3 * sizeof(unsigned int), theModel.indices(), GL_DYNAMIC_DRAW);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (action != GLFW_PRESS) return;

   if (key == GLFW_KEY_ESCAPE)
   {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
   }
   // increase red in diffuse color
   else if (key == 'R')
   {
      if(diffuse_color.x > 0.9f) {
         diffuse_color = glm::vec3(1.0f, diffuse_color.y, diffuse_color.z);
      } else {
         diffuse_color += glm::vec3(0.1f, 0, 0);
      }
   }
   // increase green in diffuse color
   else if (key == 'G')
   {
      if(diffuse_color.y > 0.9f) {
         diffuse_color = glm::vec3(diffuse_color.x, 1.0f, diffuse_color.z);
      } else {
         diffuse_color += glm::vec3(0, 0.1f, 0);
      }
   }
   // increase blue in diffuse color
   else if (key == 'B')
   {
      if(diffuse_color.z > 0.9f) {
         diffuse_color = glm::vec3(diffuse_color.x, diffuse_color.y, 1.0f);
      } else {
         diffuse_color += glm::vec3(0, 0, 0.1f);
      }
   }
   // reset diffuse color to dark gray
   else if (key == 'C')
   {
      diffuse_color = glm::vec3(0.05f,0.05f,0.05f);
   }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   // Prevent a divide by zero
   if(height == 0) height = 1;
	
   // Set Viewport to window dimensions
   glViewport(0, 0, width, height);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
   double xpos, ypos;
   glfwGetCursorPos(window, &xpos, &ypos);

   int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
   if (state == GLFW_PRESS)
   {
      mouse_pressed = true;
       int keyPress = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
       if (keyPress == GLFW_PRESS) {
          shift_pressed = true;
       } else {
          shift_pressed = false;
       }
   }
   else if (state == GLFW_RELEASE)
   {
      mouse_pressed = false;
   }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
   if(mouse_pressed) {

      // compute change in mouse movement
      float changeX = xpos - lastXPos;
      float changeY = ypos - lastYPos;

      // if shift pressed, change distance from object
      if(shift_pressed) {
         float s = 0.06f;
         dist += changeY * s;
         dist -= changeX * s;
      } else {
         azimuth -= changeX;
         elevation += changeY;

         // prevent object from flipping
         if (elevation > 89.9f)
            elevation = 89.9f;
         if (elevation < -89.9f)
            elevation = -89.9f;
      }
   }
   lastXPos = xpos;
   lastYPos = ypos;
}

static void PrintShaderErrors(GLuint id, const std::string label)
{
   std::cerr << label << " failed\n";
   GLint logLen;
   glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLen);
   if (logLen > 0)
   {
      char* log = (char*)malloc(logLen);
      GLsizei written;
      glGetShaderInfoLog(id, logLen, &written, log);
      std::cerr << "Shader log: " << log << std::endl;
      free(log);
   }
}

static std::string LoadShaderFromFile(const std::string& fileName)
{
   std::ifstream file(fileName);
   if (!file)
   {
      std::cout << "Cannot load file: " << fileName << std::endl;
      return "";
   }

   std::stringstream code;
   code << file.rdbuf();
   file.close();

   return code.str();
}

static GLuint LoadShader(const std::string& vertex, const std::string& fragment)
{
   GLint result;
   std::string vertexShader = LoadShaderFromFile(vertex);
   const char* vertexShaderRaw = vertexShader.c_str();
   GLuint vshaderId = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vshaderId, 1, &vertexShaderRaw, NULL);
   glCompileShader(vshaderId);
   glGetShaderiv(vshaderId, GL_COMPILE_STATUS, &result);
   if (result == GL_FALSE)
   {
      PrintShaderErrors(vshaderId, "Vertex shader");
      return -1;
   }

   std::string fragmentShader = LoadShaderFromFile(fragment);
   const char* fragmentShaderRaw = fragmentShader.c_str();
   GLuint fshaderId = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fshaderId, 1, &fragmentShaderRaw, NULL);
   glCompileShader(fshaderId);
   glGetShaderiv(fshaderId, GL_COMPILE_STATUS, &result);
   if (result == GL_FALSE)
   {
      PrintShaderErrors(fshaderId, "Fragment shader");
      return -1;
   }

   GLuint shaderId = glCreateProgram();
   glAttachShader(shaderId, vshaderId);
   glAttachShader(shaderId, fshaderId);
   glLinkProgram(shaderId);
   glGetShaderiv(shaderId, GL_LINK_STATUS, &result);
   if (result == GL_FALSE)
   {
      PrintShaderErrors(shaderId, "Shader link");
      return -1;
   }
   return shaderId;
}


int main(int argc, char** argv)
{
   GLFWwindow* window;

   if (!glfwInit())
   {
      return -1;
   }

   // Explicitly ask for a 4.0 context 
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   /* Create a windowed mode window and its OpenGL context */
   window = glfwCreateWindow(500, 500, "Mesh Viewer", NULL, NULL);
   if (!window)
   {
      glfwTerminate();
      return -1;
   }

   // Make the window's context current 
   glfwMakeContextCurrent(window);

   glfwSetKeyCallback(window, key_callback);
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
   glfwSetMouseButtonCallback(window, mouse_button_callback);
   glfwSetScrollCallback(window, scroll_callback);
   glfwSetCursorPosCallback(window, cursor_position_callback);

#ifndef APPLE
   if (glewInit() != GLEW_OK)
   {
      return -1;
   }
#endif

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glClearColor(0, 0, 0, 1);

   glGenBuffers(1, &theVboPosId);
   glGenBuffers(1, &theVboNormalId);
   glGenBuffers(1, &theElementbuffer);

   GLuint vaoId;
   glGenVertexArrays(1, &vaoId);
   glBindVertexArray(vaoId);

   glEnableVertexAttribArray(0); // 0 -> Sending VertexPositions to array #0 in the active shader
   glBindBuffer(GL_ARRAY_BUFFER, theVboPosId); // always bind before setting data
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

   glEnableVertexAttribArray(1); // 1 -> Sending Normals to array #1 in the active shader
   glBindBuffer(GL_ARRAY_BUFFER, theVboNormalId); // always bind before setting data
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

   LoadModel();

   //default shader (per vertex)
   //GLuint shaderId = LoadShader("../shaders/phong.vs", "../shaders/phong.fs");

   //per fragment shader
   //GLuint shaderId = LoadShader("../shaders/phong-perfrag.vs", "../shaders/phong-perfrag.fs");

   // toon shader
   GLuint shaderId = LoadShader("../shaders/phong-perfrag.vs", "../shaders/cartoon.fs");

   glUseProgram(shaderId);

   GLuint matrixParam = glGetUniformLocation(shaderId, "MVP");
   GLuint mvId= glGetUniformLocation(shaderId, "ModelViewMatrix");
   GLuint nmvId= glGetUniformLocation(shaderId, "NormalMatrix");

   // set shader inputs
   glUniform3f(glGetUniformLocation(shaderId, "Material.Ks"), 1.0, 1.0, 1.0);
   glUniform3f(glGetUniformLocation(shaderId, "Material.Ka"), 0.1, 0.1, 0.1);
   glUniform1f(glGetUniformLocation(shaderId, "Material.Shininess"), 80.0);
   glUniform4f(glGetUniformLocation(shaderId, "Light.Position"), 100.0, 100.0, 100.0, 1);
   glUniform3f(glGetUniformLocation(shaderId, "Light.La"), 1.0, 1.0, 1.0);
   glUniform3f(glGetUniformLocation(shaderId, "Light.Ld"), 1.0, 1.0, 1.0);
   glUniform3f(glGetUniformLocation(shaderId, "Light.Ls"), 1.0, 1.0, 1.0);
   glUniform3f(glGetUniformLocation(shaderId, "Light.Intensity"), 1.0, 1.0, 1.0); // for per fragment shading

   glm::mat4 transform(1.0); // initialize to identity
   glm::mat4 projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);

   glClearColor(0, 0, 0, 1);

   // Loop until the user closes the window 
   while (!glfwWindowShouldClose(window))
   {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffers

      // calculate lookfrom position
      float x = dist * sin(glm::radians(azimuth)) * cos(glm::radians(elevation));
      float y = dist * sin(glm::radians(elevation));
      float z = dist * cos(glm::radians(azimuth)) * cos(glm::radians(elevation));
      glm::vec3 lookfrom = glm::vec3(x, y, z);
      camera = glm::lookAt(lookfrom, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
      
      // set diffuse color 
      glUniform3f(glGetUniformLocation(shaderId, "Material.Kd"), diffuse_color.x, diffuse_color.y, diffuse_color.z);

      // translate and scale model to fit in view volume
      glm::vec3 min_bounds = theModel.getMinBounds();
      glm::vec3 max_bounds = theModel.getMaxBounds();
      glm::vec3 size = max_bounds - min_bounds;
      glm::vec3 center = (max_bounds + min_bounds) / 2.0f;

      float translate_x = -1 * center.x;
      float translate_y = -1 * center.y;
      float translate_z = -1 * center.z;

      glm::mat4 transl = glm::translate(glm::mat4(1.0), glm::vec3(translate_x, translate_y, translate_z));

      float max_size = std::max(size.x, size.y);
      max_size = std::max(max_size, size.z);
      float scale_factor = 2.0f / max_size;

      glm::mat4 scale = glm::scale(mat4(1.0), glm::vec3(scale_factor, scale_factor, scale_factor));
      transform = glm::translate(scale, glm::vec3(translate_x, translate_y, translate_z)); //translates and then scales

      glm::mat4 mvp = projection * camera * transform;
      glm::mat4 mv =  camera * transform;
      glm::mat3 nmv = glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2]));
      glUniformMatrix4fv(matrixParam, 1, GL_FALSE, &mvp[0][0]);
      glUniformMatrix3fv(nmvId, 1, GL_FALSE, &nmv[0][0]);
      glUniformMatrix4fv(mvId, 1, GL_FALSE, &mv[0][0]);

      // Draw primitive
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theElementbuffer);
      glDrawElements(GL_TRIANGLES, theModel.numTriangles() * 3, GL_UNSIGNED_INT, (void*)0);

      // Swap front and back buffers
      glfwSwapBuffers(window);

      // Poll for and process events
      glfwPollEvents();
   }

   glfwTerminate();
   return 0;
}


