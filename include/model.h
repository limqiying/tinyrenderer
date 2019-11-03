//
//  model.hpp
//  tinyrenderer
//
//  Created by Qi Ying Lim on 10/28/19.
//  Copyright © 2019 Tia Lim. All rights reserved.
//

#ifndef __MODEL_H__
#define __MODEL_H__

#include <stdio.h>
#include <Eigen/Dense>
#include <vector>

class Model
{
private:
    std::vector<Eigen::Vector3f> verts;
    std::vector<std::vector<int> > faces;

public:
    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();
    Eigen::Vector3f vert(int);
    std::vector<int> face(int);
};

#endif /* __MODEL_H__ */
