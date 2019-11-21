#include <Eigen/Dense>
#include "tgaimage.h"
#include "util.h"

struct IShader {
    virtual ~IShader();
    virtual Eigen::Vector4f vertex(int, int ) = 0;
    virtual bool fragment(Eigen::Vector3f, TGAColor&) = 0;
};

// struct GouraudShader : public IShader {
//     Eigen::Vector3f varying_intensity;
    
//     Eigen::Vector4f vertex(int iface, int nthvert) {
//         triangle[j] = Eigen::Vector3f(int((worldCoords[j].x() + 1.0) * image.get_width() / 2.0 + 0.5), 
//                                 int((worldCoords[j].y() + 1.0) * image.get_height() / 2.0 + 0.5), 
//                                 worldCoords[j].z());
//     }

//     bool fragment(Eigen::Vector3f barycentric, TGAColor &color) {

//     }
// };
