/*
This class computes transformation matrices required to render the image on the screen
*/

#ifndef __TRANFORMATIONS_H__
#define __TRANFORMATIONS_H__

#include <iostream>
#include <cmath>
#include <Eigen/Dense>
#include "tgaimage.h"

void lookAt(Eigen::Vector3f eye, Eigen::Vector3f center, Eigen::Vector3f up, Eigen::Matrix4f modelView)
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

    modelView = rotateToFrame * translateToFrame;
}


#endif /* __TRANFORMATIONS_H__ */