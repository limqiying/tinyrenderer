//
//  model.cpp
//  tinyrenderer
//
//  Created by Qi Ying Lim on 10/28/19.
//  Copyright © 2019 Tia Lim. All rights reserved.
//

#include "model.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std; using namespace Eigen;

Model::Model(const char *filename)
{
    ifstream in;
    in.open(filename, ifstream::in);
    if (in.fail()) return;
    std::string line;
    float raw[3];
    int i = 0;
    while (!in.eof()) {
        if (i % 10 == 0) {
            cout << verts.size() << endl;
        }
        i ++;
        getline(in, line);
        istringstream iss(line.c_str());
        char trash;
        if (line.compare(0, 2, "v ")) {
            iss >> trash;
            for (int i=0; i<3; i++) iss >> raw[i];
            Vector3f v = Vector3f(raw[0], raw[1], raw[2]);
            verts.push_back(v);
        } else if (line.compare(0, 2, "f ")) {
            std::vector<int> f;
            int itrash, idx;
            iss >> trash;
            while (iss >> idx >> trash >> itrash >> trash >> itrash) {
                idx --;
                f.push_back(idx);
            }
            faces.push_back(f);
        }
    }
    std::cerr << "# v# " << verts.size() << " f# "  << faces.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts.size();
}

int Model::nfaces() {
    return (int)faces.size();
}

std::vector<int> Model::face(int idx) {
    return faces[idx];
}

Vector3f Model::vert(int i) {
    return verts[i];
}
