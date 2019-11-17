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
const Eigen::Vector3f lightDirection = Eigen::Vector3f(0.0, 0.0, 1.0).normalized(); // define direction of light to be in the direction of lookAt


int main(int argc, char** argv) 
{
    char modelFile[50] = "resources/";
    char textureFile[50] = "resources/";
    char normalMapFile[50] = "resources/";
    const int width = 800;
    const int height = 800;
    
    TGAImage image(width, height, TGAImage::RGB);

    if (2 == argc) {    
        std::cout << "No texture file specified, drawn without textures" << std::endl;
        strcat(modelFile, argv[1]);
        drawMesh(modelFile, image, lightDirection);
    } else if (3 == argc){
        // draw textures without normal mapping

        strcat(modelFile, argv[1]);
        strcat(textureFile, argv[2]);
        
        TGAImage textureImage;
        textureImage.read_tga_file(textureFile);
        drawMesh(modelFile, image, lightDirection, &textureImage);
    } else {
        // draw textures with normal mapping

        if (4 == argc) {
            strcat(modelFile, argv[1]);
            strcat(textureFile, argv[2]);
            strcat(normalMapFile, argv[3]);
        } else {
            // otherwise use default files
            strcat(modelFile, "head.obj");
            strcat(textureFile, "head.tga");
            strcat(normalMapFile, "head_nm.tga");
        }
        TGAImage textureImage, normalMap;
        textureImage.read_tga_file(textureFile);
        normalMap.read_tga_file(normalMapFile);
        drawMesh(modelFile, image, lightDirection, &textureImage, &normalMap);
    }
    image.flip_vertically(); 
    image.write_tga_file("output.tga");
}