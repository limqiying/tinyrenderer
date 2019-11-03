#ifndef __UTILS_H__
#define __UTILS_H__

#include <iostream>
#include <cmath>
#include <Eigen/Dense>
#include "tgaimage.h"
#include "model.h"

void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, const TGAColor &color) 
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

void drawLine(const Eigen::Vector2i &p1, const Eigen::Vector2i &p2, TGAImage &image, const TGAColor &color) 
{
    drawLine(p1[0], p1[1], p2[0], p2[1], image, color);
}


void drawWireFrameMesh(const char* inputfile, const char* outputfile, int width, int height)
{
    Model* model = new Model(inputfile);
    TGAImage image(width, height, TGAImage::RGB);

    // declare buffer variables for each drawLine draw
    Eigen::Vector3f v0, v1; 
    int x0, x1, y0, y1;
    std::vector<int> face;

    // record the highest and lowest vertex value in the z-coordinate
    // this is used to color the lines
    float minZ, maxZ;
    for (int i=0; i<model ->nverts(); i++) {
        minZ = std::min(minZ, model->vert(i)[2]);
        maxZ = std::max(minZ, model->vert(i)[2]);
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
            x0 = ((v0[0] + 1.0) / 2.0) * width ;
            y0 = ((v0[1] + 1.0) / 2.0) * height;
            x1 = ((v1[0] + 1.0) / 2.0) * width ;
            y1 = ((v1[1] + 1.0) / 2.0) * height;

            // draw the line
            const float z = (1.0 - ((std::min(v0[2], v1[2]) + std::abs(minZ)) / (maxZ - minZ))) * 255;
            const TGAColor lineColor = TGAColor(z, z, z, 255);
            drawLine(x0, y0, x1, y1, image, lineColor);
        }
    }

    image.flip_vertically(); 
    image.write_tga_file(outputfile);

}

void getBarycentricCoordinates(const Eigen::Vector2i *triangle, const Eigen::Vector2i &point, Eigen::Vector3f &barycentric)
{
    // returns the barycentric coordinate of the point in terms of the input triangle
    Eigen::Vector3f u = Eigen::Vector3f(triangle[2][0]-triangle[0][0], triangle[1][0]-triangle[0][0], triangle[0][0]-point[0])
    .cross(Eigen::Vector3f(triangle[2][1]-triangle[0][1], triangle[1][1]-triangle[0][1], triangle[0][1]-point[1]));

    if (std::abs(u[2])<1) {
        barycentric[0] = -1.0;
        barycentric[1] = 1.0;
        barycentric[2] = 1.0;
    } else {
        // solution using cramer's rule to find coordinates
        barycentric[0] = 1.0 - ((u[0] + u[1]) / u[2]);
        barycentric[1] = u[1] / u[2];
        barycentric[2] = u[0] / u[2];
    }
}

bool pointInTriangle(const Eigen::Vector2i *triangle, const Eigen::Vector2i &point, Eigen::Vector3f &barycentric)
{
    getBarycentricCoordinates(triangle, point, barycentric);
    return ! (barycentric[0] < 0 || barycentric[1] < 0 || barycentric[2] < 0);
}

void drawLineTriangle(const Eigen::Vector2i *triangle, TGAImage &image, const TGAColor &color) 
{ 
    drawLine(triangle[0], triangle[1], image, color); 
    drawLine(triangle[1], triangle[2], image, color); 
    drawLine(triangle[2], triangle[0], image, color); 
    std::cout << "triangle drawn" << std::endl;
}

void drawFilledTriangle(const Eigen::Vector2i *triangle, TGAImage &image, const TGAColor &color) 
{
    // defines the bottom leftmost point of the bounding box
    int boxMin[2] = {image.get_width() - 1, image.get_height() -1}; 

    // defines the topmost point of the bounding box
    int boxMax[2] = {0, 0}; 

    // defines the upper bound of the image; triangle drawn outside this will be clipped
    int upperClamp[2] = {image.get_width() - 1, image.get_height() -1};

    for (int i=0; i<3; i++) {   // determine the bounding box parameters
        for (int j=0; j<2; j++) {
            boxMin[j] = std::max(0, std::min(boxMin[j], triangle[i][j]));
            boxMax[j] = std::min(upperClamp[j], std::max(boxMax[j], triangle[i][j]));
        }
    }

    std::cout << boxMin[0] << " " << boxMin[1]<< " " << boxMax[0]<< " " << boxMax[1] << std::endl;

    Eigen::Vector2i point;
    Eigen::Vector3f barycentric;
    for (point[0]=boxMin[0]; point[0]<=boxMax[0]; point[0]++) {
        for (point[1]=boxMin[1]; point[1]<=boxMax[1]; point[1]++) {
            if (pointInTriangle(triangle, point, barycentric)) {
                image.set(point[0], point[1], color);
            }
        }
    }
}

#endif /* __UTILS_H__ */