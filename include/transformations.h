/*
This class computes transformation matrices required to render the image on the screen
*/

#ifndef __TRANFORMATIONS_H__
#define __TRANFORMATIONS_H__

#include <iostream>
#include <cmath>
#include <Eigen/Dense>
#include "tgaimage.h"

void getModelView(Eigen::Vector3f eye, Eigen::Vector3f center, Eigen::Vector3f up, Eigen::Matrix4f &output)
{
    // computes the modelview matrix required such that the camera is looking at the -z direction
    // the model is positioned at the center
    // and the up vector reprsents the "up" direction in the world

    // first compute the basis vectors 
    Eigen::Vector3f x, y, z;
    z = (eye - center).normalized();
    x = up.cross(z).normalized();
    y = z.cross(x).normalized();

    x.transpose();
    y.transpose();
    z.transpose();

    Eigen::Matrix4f rotateToFrame = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f translateToFrame = Eigen::Matrix4f::Identity();

    // construct matrix that will rotate the curremt coordinate frame into
    // the new coordinate frame
    rotateToFrame.block<1,3>(0,0) = x.transpose();
    rotateToFrame.block<1,3>(1,0) = y;
    rotateToFrame.block<1,3>(2,0) = z;

    // construct matrix that translates from to the new coordinate system
    translateToFrame.block<3, 1>(0, 4) = -1 * center;

    output = rotateToFrame * translateToFrame;
}

void getViewport(float x, float y, int width, int height, Eigen::Matrix4f &output, int depth=255)
{
    // creates the viewport matrix that maps the bi-unit cube onto the screen
    // [-1, 1] ^ 3 -> [x, x+width] x [y, y+height] x [0, d]
    output << width / 2.0, 0, 0, x + (width / 2.0),
        0, height / 2.0, 0, y + (height / 2.0), 
        0, 0, depth / 2.0, depth / 2.0,
        0, 0, 0, 1;
}

void getProjection(float c, Eigen::Matrix4f &output)
{
    output = Eigen::Matrix4f::Identity();
    output(3, 2) = -1.0 / c;
}

Eigen::Vector4f toHomogenous(const Eigen::Vector3f &v)
{
    // converts a standard 3D Vector to homogenous coordinates
    Eigen::Vector4f homogenous;
    for (int i=0; i<3; i++) homogenous[i] = v[i];
    homogenous[3] = 1.0f;
    return homogenous;
}

Eigen::Vector3f fromHomogenous(const Eigen::Vector4f &h)
{
    Eigen::Vector3f vector;
    for (int i=0; i<3; i++) vector[i] = h[i] / h[3];
    return vector;
}


#endif /* __TRANFORMATIONS_H__ */