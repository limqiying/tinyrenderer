#ifndef __UTILS_H__
#define __UTILS_H__

#include <iostream>
#include <cmath>
#include <Eigen/Dense>
#include "tgaimage.h"
#include "model.h"
#include "transformations.h"

Eigen::Vector3f       eye(0, 0, 3);
Eigen::Vector3f    center(0, 0, 0);
Eigen::Vector3f        up(0, 1, 0);

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

bool pointInTriangle(const Eigen::Vector3f *triangle, const Eigen::Vector3f &point, Eigen::Vector3f &barycentric)
{
    getBarycentricCoordinates(triangle, point, barycentric);
    return ! (barycentric[0] < 0 || barycentric[1] < 0 || barycentric[2] < 0);
}

void drawTriangle(const Eigen::Vector3f *triangle, float *zbuffer, TGAImage &image, const Eigen::Vector3f *normals, 
                const Eigen::Vector3f &lightDirection, const Eigen::Vector2f *textures = nullptr, 
                TGAImage *textureImage = nullptr, TGAImage *normalMap = nullptr)  
{
    /*
    Given a triangle and a z-buffer, this function draws parts of the triangle that have the highest z-coordinate value
    seen so far, as determined by the z-buffer. This enables us to cull backfaces easily
    */

    // defines the boxMin to be {infinity, infinity}
    float boxMin[2] = { std::numeric_limits<float>::max(),  std::numeric_limits<float>::max()}; 
    

    // defines the boxMax to be {-infinity, -infinity}
    float boxMax[2] = {-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()}; 

    // defines the upper bound of the image; triangle drawn outside this will be clipped
    float upperClamp[2] = {image.get_width() - 1.0f, image.get_height() -1.0f};

    for (int i=0; i<3; i++) {   // determine the bounding box parameters for the triangle
        for (int j=0; j<2; j++) {
            boxMin[j] = std::max(0.0f, std::min(boxMin[j], triangle[i][j]));
            boxMax[j] = std::min(upperClamp[j], std::max(boxMax[j], triangle[i][j]));
        }
    }

    Eigen::Vector3f point, barycentric, normal;
    Eigen::Vector2f textureCoordinate;
    TGAColor pixelColor = TGAColor(255, 255, 255);
    TGAColor normalMapColor;
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
                    if (textures) textureCoordinate.setZero();
                    normal.setZero();
                    for (int i=0; i<3; i++) {
                        // interpolate texture coordinate and normal from their values at the vertices
                        if (textureImage && textures) textureCoordinate += barycentric[i] * textures[i];
                        normal += barycentric[i] * normals[i];
                    }

                    // if textures are provided, color by texture image, otherwise set white
                    if (textureImage && textures) {
                        pixelColor = textureImage->get((textureCoordinate.x()) * textureImage->get_width(), (1.0f - textureCoordinate.y()) * textureImage->get_height());
                    } 
                    if (normalMap) {
                        // if normal map is specified, grab the color in then normal map
                        // then overwrite the normal with the value in the normal map
                        normalMapColor = normalMap->get((textureCoordinate.x()) * textureImage->get_width(), (1.0f - textureCoordinate.y()) * textureImage->get_height());
                        normal.setZero();
                        for (int i=0; i<3; i++) normal[i] = normalMapColor[i];
                        normal.normalize();
                    }

                    intensity = normal.dot(lightDirection);
                    // draw the pixel of the specific texture color
                    image.set(point.x(), point.y(), pixelColor * intensity);

                }
            }
        }
    }
}

void drawMesh(const char* inputFile, TGAImage &image, const Eigen::Vector3f &lightDirection, TGAImage *textureImage = nullptr, TGAImage *normalMap = nullptr)
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
   Eigen::Matrix4f projection, viewport, modelView;
   getModelView(eye, center, up, modelView);
   getProjection((eye - center).norm(), projection);
   getViewport(0.0, 0.0, image.get_width(), image.get_height(), viewport);

   for (int i=0; i< model->nfaces(); i++) {
       face = model->face(i);

       for (int j=0; j<3; j++) {
           worldCoords[j] = model -> vert(face[j].vertexIndex);    // get the coordinates of the triangle in world coordinates
           textures[j] = model -> texture(face[j].textureIndex);
           normals[j] = model -> normal(face[j].normalIndex);
           // then tranform the coordinates to screenspace, with the additional z-coordinate value
            triangle[j] = fromHomogenous(viewport * projection * modelView * toHomogenous(worldCoords[j]));
       }
        drawTriangle(triangle, zbuffer, image, normals, lightDirection, textures, textureImage, normalMap);
   }
   delete model;
}

#endif  /*__UTILS_H__ */