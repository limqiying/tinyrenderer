#ifndef __UTILS_H__
#define __UTILS_H__

#include <iostream>
#include <cmath>
#include <Eigen/Dense>
#include "tgaimage.h"
#include "model.h"
#include "transformations.h"

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

void getBarycentricCoordinates(const Eigen::Vector3f *triangle, const Eigen::Vector3f &point, Eigen::Vector3f &barycentric)
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

bool pointInTriangle(const Eigen::Vector3f *triangle, const Eigen::Vector3f &point, Eigen::Vector3f &barycentric)
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
    /*
    Given a triangle, this function draw a filled triangle (vs a line triangle) of the input color,
    and outputs it to the TGAImage parameter
    */

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

    Eigen::Vector2i point;
    Eigen::Vector3f barycentric;

    // for every pixel in the bounding box, check if the pixel if in the triangle
    // if it is in the triangle, color the pixel
    for (point[0]=boxMin[0]; point[0]<=boxMax[0]; point[0]++) {
        for (point[1]=boxMin[1]; point[1]<=boxMax[1]; point[1]++) {
            if (pointInTriangle(triangle, point, barycentric)) {
                image.set(point[0], point[1], color);
            }
        }
    }
}

void drawFilledTriangleZ(const Eigen::Vector3f *triangle, float *zbuffer, TGAImage &image, const Eigen::Vector3f *normals, const Eigen::Vector3f &lightDirection) 
{
    /*
    Given a triangle and a z-buffer, this function draws parts of the triangle that have the highest z-coordinate value
    seen so far, as determined by the z-buffer. This enables us to cull backfaces easily
    */

    // defines the boxMin to be {infinity, infinity}
    float boxMin[2] = { std::numeric_limits<float>::max(),  std::numeric_limits<float>::max()}; 
    

    // defines the boxMin to be {-infinity, -infinity}
    float boxMax[2] = {-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()}; 

    // defines the upper bound of the image; triangle drawn outside this will be clipped
    float upperClamp[2] = {image.get_width() - 1, image.get_height() -1};

    for (int i=0; i<3; i++) {   // determine the bounding box parameters for the triangle
        for (int j=0; j<2; j++) {
            boxMin[j] = std::max(0.0f, std::min(boxMin[j], triangle[i][j]));
            boxMax[j] = std::min(upperClamp[j], std::max(boxMax[j], triangle[i][j]));
        }
    }

    Eigen::Vector3f point, barycentric, normal;
    TGAColor pixelColor = TGAColor(255, 255, 255);
    float intensity;

    // for every pixel in the bounding box
    for (point[0]=boxMin[0]; point[0]<=boxMax[0]; point[0]++) {
        for (point[1]=boxMin[1]; point[1]<=boxMax[1]; point[1]++) {

            // check if the pixel lies in the triangle in the screen
            // and grab the barycentric coordinates
            if (pointInTriangle(triangle, point, barycentric)) {

                // initialize the z-coordinate of the point to 0
                point[2] = 0.0;

                // compute the z-coordinate of the point in terms of its barycentric coordinates
                for (int i=0; i<3; i++) point[2] += triangle[i][2] * barycentric[i];

                // if the point is infront (higher z-value) of the previously-drawn point, then color the pixel in
                if (zbuffer[int(point.x() + point.y() * image.get_width())] < point.z()) {

                    // update the maximum z-coordinate
                    zbuffer[int(point.x() + point.y() * image.get_width())] = point.z();

                    // reset textureCoordinate and normal vectors
                    normal.setZero();
                    for (int i=0; i<3; i++) {
                        // interpolate texture coordinate and normal from their values at the vertices
                        normal += barycentric[i] * normals[i];
                    }

                    intensity = normal.dot(lightDirection);
                    // draw the pixel of the specific texture color
                    image.set(point.x(), point.y(), pixelColor * intensity);

                }
            }
        }
    }
}

void drawFilledTriangleZ(const Eigen::Vector3f *triangle, float *zbuffer, TGAImage &image, const Eigen::Vector3f *normals, const Eigen::Vector2f *textures, TGAImage &textureImage, const Eigen::Vector3f &lightDirection)  
{
    /*
    Given a triangle and a z-buffer, this function draws parts of the triangle that have the highest z-coordinate value
    seen so far, as determined by the z-buffer. This enables us to cull backfaces easily
    */

    // defines the boxMin to be {infinity, infinity}
    float boxMin[2] = { std::numeric_limits<float>::max(),  std::numeric_limits<float>::max()}; 
    

    // defines the boxMin to be {-infinity, -infinity}
    float boxMax[2] = {-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()}; 

    // defines the upper bound of the image; triangle drawn outside this will be clipped
    float upperClamp[2] = {image.get_width() - 1, image.get_height() -1};

    for (int i=0; i<3; i++) {   // determine the bounding box parameters for the triangle
        for (int j=0; j<2; j++) {
            boxMin[j] = std::max(0.0f, std::min(boxMin[j], triangle[i][j]));
            boxMax[j] = std::min(upperClamp[j], std::max(boxMax[j], triangle[i][j]));
        }
    }

    Eigen::Vector3f point, barycentric, normal;
    Eigen::Vector2f textureCoordinate;
    TGAColor pixelColor;
    float intensity;

    // for every pixel in the bounding box
    for (point[0]=boxMin[0]; point[0]<=boxMax[0]; point[0]++) {
        for (point[1]=boxMin[1]; point[1]<=boxMax[1]; point[1]++) {

            // check if the pixel lies in the triangle in the screen
            // and grab the barycentric coordinates
            if (pointInTriangle(triangle, point, barycentric)) {

                // initialize the z-coordinate of the point to 0
                point[2] = 0.0;

                // compute the z-coordinate of the point in terms of its barycentric coordinates
                for (int i=0; i<3; i++) point[2] += triangle[i][2] * barycentric[i];

                // if the point is infront (higher z-value) of the previously-drawn point, then color the pixel in
                if (zbuffer[int(point.x() + point.y() * image.get_width())] < point.z()) {

                    // update the maximum z-coordinate
                    zbuffer[int(point.x() + point.y() * image.get_width())] = point.z();

                    // reset textureCoordinate and normal vectors
                    textureCoordinate.setZero();
                    normal.setZero();
                    for (int i=0; i<3; i++) {
                        // interpolate texture coordinate and normal from their values at the vertices
                        textureCoordinate += barycentric[i] * textures[i];
                        normal += barycentric[i] * normals[i];
                    }

                    pixelColor = textureImage.get((1.0f - textureCoordinate.x()) * textureImage.get_width(), (1.0f - textureCoordinate.y()) * textureImage.get_height());
                    intensity = normal.dot(lightDirection);
                    // draw the pixel of the specific texture color
                    image.set(point.x(), point.y(), pixelColor * intensity);

                }
            }
        }
    }
}

void rasterize1D(Eigen::Vector2i p0, Eigen::Vector2i p1, TGAImage &image, const TGAColor &color, int ybuffer[])
{
    if (p0.x() > p1.x()) {
        // ensure that p0 has the smaller x-coordinate of the two
        std::swap(p0, p1);
    }
    
    // initialize temporary buffer values
    float t;
    int y;

    for (int x=p0.x(); x<p1.x(); x++) {     // from the left to the right of the line

        // compute the y-coordinate of the pixel
        t = (x - p0.x()) / (float)(p1.x() - p0.x());
        y = p0.y() * (1.0 - t) + p1.y() * t;

        if (ybuffer[x] < y) {
            // set ybuffer[x] to be the largest y-value seen so far of all lines drawn
            ybuffer[x] = y;
            // if this line is the highest seen (ie highest y-value at this x position), 
            // color the line this color
            image.set(x, 0, color);
        }
    }
}

void drawWireFrameMesh(const char* inputFile, TGAImage &image)
{
    /*
    This function takes an onj file path and draws the wireframe mesh of the model
    */

    Model* model = new Model(inputFile);

    // declare buffer variables for each drawLine draw
    Eigen::Vector3f v0, v1;     
    int x0, x1, y0, y1;
    std::vector<FaceInfo> face;

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
            v0 = model -> vert(face[j].vertexIndex);
            v1 = model -> vert(face[(j + 1) % 3].vertexIndex);

            // assuming that model is centered at (0, 0)
            // shift vertex value from [-1, 1] to [0, 1]
            // scale to [0, height] / [0, width]
            x0 = ((v0[0] + 1.0) / 2.0) * image.get_width();
            y0 = ((v0[1] + 1.0) / 2.0) * image.get_height();
            x1 = ((v1[0] + 1.0) / 2.0) * image.get_width();
            y1 = ((v1[1] + 1.0) / 2.0) * image.get_height();

            // draw the line
            const float z = (1.0 - ((std::min(v0[2], v1[2]) + std::abs(minZ)) / (maxZ - minZ))) * 255;
            const TGAColor lineColor = TGAColor(z, z, z, 255);
            drawLine(x0, y0, x1, y1, image, lineColor);
        }
    }
}


void drawTriangleMesh(const char* inputFile, TGAImage &image)
{
    /*
    This function draws the obj file specified by inputFile by drawing each of its individual faces
    there is an erroneous z-culling algorithm implemented here, which basically draws the triangles
    that are facing the front. This causes some problems with drawing surfaces that are concave but front-facing.
    This problem is solved in the function drawTriangleMeshZ, which uses a Z-buffer.
    */

    Model* model = new Model(inputFile);
    Eigen::Vector3f lightDirection = Eigen::Vector3f(0, 0, 1.0); // define direction of light to be in the direction of lookAt

    // initialize buffer variables
    std::vector<FaceInfo> face;
    Eigen::Vector2i screenCoords[3];
    Eigen::Vector3f worldCoords[3], normal;
    float intensity;

    // for each face of the model
    for (int i=0; i<model->nfaces(); i++) {
        face = model->face(i);

        // for each vertex of the face
        for (int j=0; j<3; j++) {
            worldCoords[j] = model -> vert(face[j].vertexIndex);

            // flatten the model and scale coordinates to screen space
            screenCoords[j] = Eigen::Vector2i((worldCoords[j][0] + 1.0) * image.get_width() / 2.0, (worldCoords[j][1] + 1.0) * image.get_height() / 2);
        }

        // define the vector that is normal to the triangle face
        normal = (worldCoords[1] - worldCoords[0])
                                .cross(worldCoords[2] - worldCoords[0])
                                .normalized();

        intensity = normal.dot(lightDirection);
        if (intensity > 0.0) {
            drawFilledTriangle(&screenCoords[0], image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
        }
    }
}

void drawTriangleMeshZ(const char* inputFile, TGAImage &image, TGAImage &textureImage, const Eigen::Vector3f &lightDirection)
{
    /*
    Similar to drawTriangleMesh, this function draws the obj file, but uses a z-buffer to ensure that only the
    faces cloest to the camera are seen.
    */

   Model* model = new Model(inputFile); 

    // define the zbuffer, which is initialized to contain all -infinity value
    // the zbuffer will hold the largest z-coordinate value seen at that particular x, y point
   float *zbuffer = new float[image.get_width() * image.get_height()];
   for (int i=0; i<image.get_width()*image.get_height(); i++) zbuffer[i] = -std::numeric_limits<float>::max();

   std::vector<FaceInfo> face;
   Eigen::Vector3f triangle[3], normals[3], worldCoords[3];
   Eigen::Vector2f textures[3];
   Eigen::Matrix4f projection, viewport;
   getProjection(3, projection);
   getViewport(0.0, 0.0, image.get_width(), image.get_height(), viewport);

   for (int i=0; i< model->nfaces(); i++) {
       face = model->face(i);

       for (int j=0; j<3; j++) {
           worldCoords[j] = model -> vert(face[j].vertexIndex);    // get the coordinates of the triangle in world coordinates
           textures[j] = model -> texture(face[j].textureIndex);
           normals[j] = model -> normal(face[j].normalIndex);
           // then tranform the coordinates to screenspace, with the additional z-coordinate value
           triangle[j] = Eigen::Vector3f(int((worldCoords[j].x() + 1.0) * image.get_width() / 2.0 + 0.5), 
                                            int((worldCoords[j].y() + 1.0) * image.get_height() / 2.0 + 0.5), 
                                            worldCoords[j].z());
            // triangle[j] = fromHomogenous(viewport * projection * toHomogenous(worldCoords[j]));
       }
        drawFilledTriangleZ(triangle, zbuffer, image, normals, textures, textureImage, lightDirection);
   }
}

void drawTriangleMeshZ(const char* inputFile, TGAImage &image, const Eigen::Vector3f &lightDirection)
{
    /*
    Similar to drawTriangleMesh, this function draws the obj file, but uses a z-buffer to ensure that only the
    faces cloest to the camera are seen.git 
    */

   Model* model = new Model(inputFile); 

    // define the zbuffer, which is initialized to contain all -infinity value
    // the zbuffer will hold the largest z-coordinate value seen at that particular x, y point
   float *zbuffer = new float[image.get_width() * image.get_height()];
   for (int i=0; i<image.get_width()*image.get_height(); i++) zbuffer[i] = -std::numeric_limits<float>::max();

   std::vector<FaceInfo> face;
   Eigen::Vector3f triangle[3], worldCoords[3], normals[3];
   Eigen::Matrix4f projection, viewport;
   getProjection(3, projection);
   getViewport(0.0, 0.0, image.get_width(), image.get_height(), viewport);

   for (int i=0; i< model->nfaces(); i++) {
       face = model->face(i);

       for (int j=0; j<3; j++) {
           worldCoords[j] = model -> vert(face[j].vertexIndex);    // get the coordinates of the triangle in world coordinates
           normals[j] = model -> normal(face[j].normalIndex);
           // then tranform the coordinates to screenspace, with the additional z-coordinate value
           triangle[j] = Eigen::Vector3f(int((worldCoords[j].x() + 1.0) * image.get_width() / 2.0 + 0.5), 
                                            int((worldCoords[j].y() + 1.0) * image.get_height() / 2.0 + 0.5), 
                                            worldCoords[j].z());
            // triangle[j] = fromHomogenous(viewport * projection * toHomogenous(worldCoords[j]));
       }
        drawFilledTriangleZ(triangle, zbuffer, image, normals, lightDirection);
   }
}

#endif  /*__UTILS_H__ */