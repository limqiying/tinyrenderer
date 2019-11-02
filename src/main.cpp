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
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const char* output = "./output.tga";
const int width = 800;
const int height = 800;
Model *model = NULL;

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    // uses Bresenham's Line Drawing Algorithm to draw a line from 
    // (x0, y0) to (x1, y1)

    bool steep = false;
    if(std::abs(x0-x1) < std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = std::abs(dy) * 2;
    float error2 = 0;
    int y = y0;
    for (int x=x0; x<=x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

int main(int argc, char** argv) {
    model = new Model("resources/head.obj");
    TGAImage image(width, height, TGAImage::RGB);

    Eigen::Vector3f v0, v1;
    int x0, x1, y0, y1;
    std::vector<int> face;
    for (int i=0; i< model->nfaces(); i++) {
        face = model->face(i);
        for (int j=0; j<3; j++) {
            v0 = model -> vert(face[j]);
            v1 = model -> vert(face[(j + 1) % 3]);
            x0 = (v0.x() + 1.0) * width / 2.0 ;
            y0 = (v0.y() + 1.0) * height / 2.0;
            x1 = (v1.x() + 1.0) * width / 2.0 ;
            y1 = (v1.y() + 1.0) * height / 2.0;
            line(x0, y0, x1, y1, image, white);
        }
    }
    image.flip_vertically(); 
    image.write_tga_file(output);

}

//     line(13, 20, 80, 40, image, white);
//     line(20, 13, 40, 80, image, red);
//     image.flip_vertically(); 
//     image.write_tga_file(output);
//     return 0;
