
#include "mesh.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <cmath>
#include <iostream>
#include <fstream>

using namespace std;
using namespace glm;
using namespace agl;

Mesh::Mesh() 
{
   num_vertices = 0;
   num_faces = 0;
   positions_arr = NULL;
   normals_arr = NULL;
   indices_arr = NULL;
   min_bound = glm::vec3(0);
   max_bound = glm::vec3(0);
}

Mesh::~Mesh()
{
   delete[] positions_arr;
   delete[] normals_arr;
   delete[] indices_arr;
}

bool Mesh::loadPLY(const std::string& filename)
{
   ifstream file(filename);
   if(!file) {
      cout << "Cannot load file: " << filename << endl;
      return false;
   }
   string ply;
   file >> ply;

   if(ply != "ply") {
      cout << "Not valid ply file" << endl;
      return false;
   }

   string word;
   while (word != "vertex") {
      file >> word;
   }
   file >> num_vertices;

   while (word != "face") {
      file >> word;
   }
   file >> num_faces;

   while (word != "end_header") {
      file >> word;
   }

   positions_arr = new float[num_vertices * 3];
   normals_arr = new float[num_vertices * 3];

   float max = std::numeric_limits<float>::infinity();
   float min = -max;

   float min_x = max;
   float min_y = max;
   float min_z = max;

   float max_x = min;
   float max_y = min;
   float max_z = min;

   float pos;
   float norm;

   int pos_index = 0;
   int norm_index = 0;
   
   for(int i = 0; i < num_vertices * 3 * 2; i++) {

      if(i % 6 == 0 || i % 6 == 1 || i % 6 == 2) {
         file >> pos;
         positions_arr[pos_index] = pos;
         pos_index++;

         if(i % 6 == 0) { // x
            if(pos < min_x)
               min_x = pos;
            if(pos > max_x)
               max_x = pos;
         }

         if(i % 6 == 1) { // y
            if(pos < min_y)
               min_y = pos;
            if(pos > max_y)
               max_y = pos;
         }

         if(i % 6 == 2) { // z
            if(pos < min_z)
               min_z = pos;
            if(pos > max_z)
               max_z = pos;
         }
      } else {
         file >> norm;
         normals_arr[norm_index] = norm;
         norm_index++;
      }
   }
   min_bound = glm::vec3(min_x, min_y, min_z);
   max_bound = glm::vec3(max_x, max_y, max_z);

   indices_arr = new unsigned int[num_faces * 3];

   int num;
   int index = 0;
   for(int i = 0; i < num_faces; i++) {
      file >> num; // should be 3 for our files since they only have triangles
      for(int j = 0; j < num; j++) {
         file >> indices_arr[index];
         index++;
      }
   }
   file.close();
   return true;
}

glm::vec3 Mesh::getMinBounds() const
{
  return min_bound;
}

glm::vec3 Mesh::getMaxBounds() const
{
  return max_bound;
}

int Mesh::numVertices() const
{
   return num_vertices;
}

int Mesh::numTriangles() const
{
   return num_faces;
}

float* Mesh::positions() const
{
   return positions_arr;
}

float* Mesh::normals() const
{
   return normals_arr;
}

unsigned int* Mesh::indices() const
{
   return indices_arr; 
}

