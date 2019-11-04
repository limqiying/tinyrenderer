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
    FaceInfo(int v, int t): vertexIndex(v), textureIndex(t) {}
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

public:
    Model(const char *filename);
    ~Model();
    int nverts();   // number of vertices
    int nfaces();   // number of faces
    int ntextures();
    Eigen::Vector3f vert(int);  // grabs the vertex at the specified index
    Eigen::Vector2f texture(int);
    std::vector<FaceInfo> face(int); // grabs the face at the specified index
};

#endif /* __MODEL_H__ */
