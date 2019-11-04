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

int main(int argc, char** argv) 
{
    char modelFile[20] = "resources/";

    // if filename is provided in the commandline, render that file
    // otherwise, default to head.obj
    if (2==argc) {
        strcat(modelFile, argv[1]);
    } else {
        strcat(modelFile, "head.obj");
    }
    
    const int width = 800;
    const int height = 800;

    TGAImage image(width, height, TGAImage::RGB);

    drawTriangleMeshZ(modelFile, image);
    image.flip_vertically(); 
    image.write_tga_file("output.tga");
}