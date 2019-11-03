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

int main(int argc, char** argv) 
{
    const int width = 200;
    const int height = 200;
    // drawWireFrameMesh("resources/head.obj", "output.tga", 200, 200);
    TGAImage image(width, height, TGAImage::RGB);

    Eigen::Vector2i t0[3] = {Eigen::Vector2i(10, 70),   Eigen::Vector2i(50, 160),  Eigen::Vector2i(70, 80)}; 
    Eigen::Vector2i t1[3] = {Eigen::Vector2i(180, 50),  Eigen::Vector2i(150, 1),   Eigen::Vector2i(70, 180)}; 
    Eigen::Vector2i t2[3] = {Eigen::Vector2i(180, 150), Eigen::Vector2i(120, 160), Eigen::Vector2i(130, 180)}; 
    drawLineTriangle(t0[0], t0[1], t0[2], image, red); 
    drawLineTriangle(t1[0], t1[1], t1[2], image, white); 
    drawLineTriangle(t2[0], t2[1], t2[2], image, green);

    image.flip_vertically(); 
    image.write_tga_file("output.tga");
}