#include "tgaimage.h"
#include <Eigen/Dense>

extern Eigen::Matrix4f ModelView;
extern Eigen::Matrix4f Viewport;
extern Eigen::Matrix4f Projection;

void viewport(int x, int y, int w, int h);
void projection(float coeff=0.f); // coeff = -1/c
void lookat(Eigen::Vector3f eye, Eigen::Vector3f center, Eigen::Vector3f up);

struct IShader {
    virtual ~IShader();
    virtual Eigen::Vector4f vertex(int iface, int nthvert) = 0;
    virtual bool fragment(Eigen::Vector3f bar, TGAColor &color) = 0;
};

void triangle(Eigen::Vector4f *pts, IShader &shader, TGAImage &image, TGAImage &zbuffer);