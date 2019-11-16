//
//  model.hpp
//  tinyrenderer
//
//  Created by Qi Ying Lim on 10/28/19.
//  Copyright © 2019 Tia Lim. All rights reserved.
//

#ifndef __MODEL_H__
#define __MODEL_H__

#include <stdio.h>
#include <Eigen/Dense>
#include <vector>

struct FaceInfo {
    int vertexIndex;
    int textureIndex;
    int normalIndex;
    FaceInfo(int v, int t, int n): vertexIndex(v), textureIndex(t), normalIndex(n) {}
};

class Model
{
private:
    // a vector that holds all the vertices in the model
    std::vector<Eigen::Vector3f> verts;

    // a vector that holds all the textures in the model
    std::vector<Eigen::Vector2f> textures;

    // a vector that holds all the faces in the model
    // each face is a length-3 vector, each element in the vector representing
    // the index of a vertex of the face
    std::vector<std::vector<FaceInfo> > faces;

    // a vector that holds all the normals of the vertices in the model
    // this is used to do Gouraud shading, where the normal per point in each face
    // is an interpolation of the three vertices' normals
    std::vector<Eigen::Vector3f> normals;

public:
    Model(const char *filename);
    ~Model();
    int nverts();   // number of vertices
    int nfaces();   // number of faces
    int ntextures();
    int nnormals();
    Eigen::Vector3f vert(int);  // grabs the vertex at the specified index
    Eigen::Vector2f texture(int);   // grabs the texture at the specific index
    Eigen::Vector3f normal(int); // grabs the normal at the specific index
    std::vector<FaceInfo> face(int); // grabs the face at the specified index
};

#endif /* __MODEL_H__ */
