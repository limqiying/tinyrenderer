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
const int width = 800;
const int height = 800;
Model *model = NULL;

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) 
{
    // uses Bresenham's Line Drawing Algorithm to draw a line from (x0, y0) to (x1, y1)
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

void drawWireFrameMesh(const char* inputfile, const char* outputfile)
{
    model = new Model(inputfile);
    TGAImage image(width, height, TGAImage::RGB);

    // declare buffer variables for each line draw
    Eigen::Vector3f v0, v1; 
    int x0, x1, y0, y1;
    std::vector<int> face;

    // record the highest and lowest vertex value in the z-coordinate
    // this is used to color the lines
    float minZ, maxZ;
    for (int i=0; i<model ->nverts(); i++) {
        minZ = std::min(minZ, model->vert(i).z());
        maxZ = std::max(minZ, model->vert(i).z());
    }

    for (int i=0; i<model->nfaces(); i++) {

        face = model->face(i);

        for (int j=0; j<3; j++) {
            // get one of three lines of the triangular face
            v0 = model -> vert(face[j]);
            v1 = model -> vert(face[(j + 1) % 3]);

            // assuming that model is centered at (0, 0)
            // shift vertex value from [-1, 1] to [0, 1]
            // scale to [0, height] / [0, width]
            x0 = ((v0.x() + 1.0) / 2.0) * width ;
            y0 = ((v0.y() + 1.0) / 2.0) * height;
            x1 = ((v1.x() + 1.0) / 2.0) * width ;
            y1 = ((v1.y() + 1.0) / 2.0) * height;

            // draw the line
            const float z = (1.0 - ((std::min(v0.z(), v1.z()) + std::abs(minZ)) / (maxZ - minZ))) * 255;
            const TGAColor lineColor = TGAColor(z, z, z, 255);
            line(x0, y0, x1, y1, image, lineColor);
        }
    }

    image.flip_vertically(); 
    image.write_tga_file(outputfile);

}

int main(int argc, char** argv) 
{
    drawWireFrameMesh("resources/head.obj", "output.tga");
}