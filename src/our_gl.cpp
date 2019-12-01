#include <cmath>
#include <limits>
#include <cstdlib>
#include <Eigen/Dense>
#include "our_gl.h"

Eigen::Matrix4f ModelView;
Eigen::Matrix4f Viewport;
Eigen::Matrix4f Projection;

IShader::~IShader() {}

void viewport(int x, int y, int w, int h) {
    Viewport = Eigen::Matrix4f::Identity();
    Viewport(0, 3) = x+w/2.f;
    Viewport(1, 3) = y+h/2.f;
    Viewport(2, 3) = 255.f/2.f;
    Viewport(0, 0) = w/2.f;
    Viewport(1, 1) = h/2.f;
    Viewport(2, 2) = 255.f/2.f;
}

void projection(float coeff) {
    Projection = Eigen::Matrix4f::Identity();
    Projection(3, 2) = coeff;
}

void lookat(Eigen::Vector3f eye, Eigen::Vector3f center, Eigen::Vector3f up) {
    Eigen::Vector3f z = (eye-center).normalized();
    Eigen::Vector3f x = up.cross(z).normalized();
    Eigen::Vector3f y = z.cross(x).normalized();
    ModelView = Eigen::Matrix4f::Identity();
    for (int i=0; i<3; i++) {
        ModelView(0, i) = x[i];
        ModelView(1, i) = y[i];
        ModelView(2, i) = z[i];
        ModelView(i, 3) = -center[i];
    }
}

Eigen::Vector3f barycentric(Eigen::Vector2f A, Eigen::Vector2f B, Eigen::Vector2f C, Eigen::Vector2f P) {
    Eigen::Vector3f s[2];
    for (int i=2; i--; ) {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    Eigen::Vector3f u = s[0].cross(s[1]);
    if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return Eigen::Vector3f(1.f-(u.x()+u.y())/u.z(), u.y()/u.z(), u.x()/u.z());
    return Eigen::Vector3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void triangle(Eigen::Vector4f *pts, IShader &shader, TGAImage &image, TGAImage &zbuffer) {
    Eigen::Vector2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Eigen::Vector2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::min(bboxmin[j], pts[i][j]/pts[i][3]);
            bboxmax[j] = std::max(bboxmax[j], pts[i][j]/pts[i][3]);
        }
    }
    Eigen::Vector2i P;
    TGAColor color;
    for (P.x()=bboxmin.x(); P.x()<=bboxmax.x(); P.x()++) {
        for (P.y()=bboxmin.y(); P.y()<=bboxmax.y(); P.y()++) {
            Eigen::Vector4f first, second, third, p;
            first = pts[0] / pts[0][3];
            second = pts[0] / pts[1][3];
            third = pts[0] / pts[2][3];
            Eigen::Vector3f c = barycentric(Eigen::Vector2f(first[0], first[1]), Eigen::Vector2f(second[0], second[1]), Eigen::Vector2f(third[0], third[1]), Eigen::Vector2f(P[0], P[1]));
            float z = pts[0][2] * c.x() + pts[1][2] * c.y() + pts[2][2] * c.z();
            float w = pts[0][3] * c.x() + pts[1][3] * c.y() + pts[2][3] * c.z();
            int frag_depth = std::max(0, std::min(255, int(z/w+.5)));
            if (c.x()<0 || c.y()<0 || c.z()<0 || zbuffer.get(P.x(), P.y())[0]>frag_depth) continue;
            bool discard = shader.fragment(c, color);
            if (!discard) {
                zbuffer.set(P.x(), P.y(), TGAColor(frag_depth));
                image.set(P.x(), P.y(), color);
            }
        }
    }
}