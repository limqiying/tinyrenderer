#ifndef __ISHADER__
#define __ISHADER__

#include tgaimage.h
#include <Eigen/Dense>

struct IShader {
    virtual ~IShader();
    virtual Eigen::Vector3i vertex(int iface, int nthvert) = 0;
    virtual bool framment(Eigen::Vector3f bar, TGAColor &color) = 0;
}

#endif /*__ISHADER__*/