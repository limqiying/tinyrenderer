//
//  model.hpp
//  tinyrenderer
//
//  Created by Qi Ying Lim on 10/28/19.
#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include <string>
#include <Eigen/Dense>
#include "tgaimage.h"

class Model {
private:
    // a vector that holds all the vertices in the model
    std::vector<Eigen::Vector3f> verts_;

    // a vector that holds all the faces in the model
    // each face is a length-3 vector, each element in the vector representing
    // vertex/uv/normal
    std::vector<std::vector<Eigen::Vector3i> > faces_; 

    // a vector that holds all the normals of the vertices in the model
    // this is used to do Gouraud shading, where the normal per point in each face
    // is an interpolation of the three vertices' normals
    std::vector<Eigen::Vector3f> norms_;
    std::vector<Eigen::Vector2f> uv_;
    TGAImage diffusemap_;
    TGAImage normalmap_;
    TGAImage specularmap_;
    void load_texture(std::string filename, const char *suffix, TGAImage &img);
public:
    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();
    Eigen::Vector3f normal(int iface, int nthvert); // grabs the normal at the specific index
    Eigen::Vector3f normal(Eigen::Vector2f uv);
    Eigen::Vector3f vert(int i);    // grabs the vertex at the specified index
    Eigen::Vector3f vert(int iface, int nthvert);
    Eigen::Vector2f uv(int iface, int nthvert); // grabs the texture at the specific index
    TGAColor diffuse(Eigen::Vector2f uv);
    float specular(Eigen::Vector2f uv);
    std::vector<int> face(int idx); // grabs the face at the specified index
};
#endif //__MODEL_H__