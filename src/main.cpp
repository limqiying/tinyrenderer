//
//  main.cpp
//  tinyrenderer
//
//  Created by Qi Ying Lim on 10/19/19.
//  Copyright © 2019 Tia Lim. All rights reserved.
//

#include <iostream>
#include <cmath>
#include <Eigen/Dense>
#include "tgaimage.h"
#include "utils.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
const Eigen::Vector3f lightDirection = Eigen::Vector3f(0.0, 0.0, 1.0); // define direction of light to be in the direction of lookAt


int main(int argc, char** argv) 
{
    char modelFile[50] = "resources/";
    const int width = 800;
    const int height = 800;
    
    TGAImage image(width, height, TGAImage::RGB);

    if (2 == argc) {    
        std::cout << "No texture file specified, drawn without textures" << std::endl;
        strcat(modelFile, argv[1]);
        drawTriangleMeshZ(modelFile, image, lightDirection);
    } else {
        // otherwise, draw textures
        char textureFile[50] = "resources/";
        if (3 == argc) {
            // if target model and texture files are specified, use then
            strcat(modelFile, argv[1]);
            strcat(textureFile, argv[2]);
        } else {
            // otherwise use default files
            strcat(modelFile, "head.obj");
            strcat(textureFile, "head.tga");
        }
        TGAImage textureImage;
        textureImage.read_tga_file(textureFile);
        drawTriangleMeshZ(modelFile, image, textureImage, lightDirection);
    }

    image.flip_vertically(); 
    image.write_tga_file("output.tga");
}