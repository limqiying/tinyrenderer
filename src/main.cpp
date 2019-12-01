#include <vector>
#include <iostream>
#include <Eigen/Dense>

#include "tgaimage.h"
#include "model.h"
#include "our_gl.h"

Model *model     = NULL;
const int width  = 800;
const int height = 800;

Eigen::Vector3f light_dir(1,1,1);
Eigen::Vector3f       eye(1,1,3);
Eigen::Vector3f    center(0,0,0);
Eigen::Vector3f        up(0,1,0);

struct GouraudShader : public IShader {
    Eigen::Vector3f varying_intensity; // written by vertex shader, read by fragment shader

    virtual Eigen::Vector4f vertex(int iface, int nthvert) {
        varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert).dot(light_dir)); // get diffuse lighting intensity
        Eigen::Vector3f vertex = model->vert(iface, nthvert);
        Eigen::Vector4f gl_Vertex;
        gl_Vertex << vertex[0], vertex[1], vertex[2], 1.0f;
        return Viewport*Projection*ModelView*gl_Vertex; // transform it to screen coordinates
    }

    virtual bool fragment(Eigen::Vector3f barycentric, TGAColor &color) {
        float intensity = varying_intensity.dot(barycentric);   // interpolate intensity for the current pixel
        color = TGAColor(255, 255, 255)*intensity; 
        return false;                   
    }           
};

int main(int argc, char** argv) {
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("resouces/head.obj");
    }

    lookat(eye, center, up);
    viewport(width/8, height/8, width*3/4, height*3/4);
    projection(-1.f/(eye-center).norm());
    light_dir.normalize();

    TGAImage image  (width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    GouraudShader shader;
    for (int i=0; i<model->nfaces(); i++) {
        Eigen::Vector4f screen_coords[3];
        for (int j=0; j<3; j++) {
            screen_coords[j] = shader.vertex(i, j);
        }
        triangle(screen_coords, shader, image, zbuffer);
    }

    image.  flip_vertically(); // to place the origin in the bottom left corner of the image
    zbuffer.flip_vertically();
    image.  write_tga_file("output.tga");
    zbuffer.write_tga_file("zbuffer.tga");

    delete model;
    return 0;
}