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

    //  open file
    in.open(filename, ifstream::in);

    // check success of file read    
    if (in.fail()) {
        cout << "failed" << endl;
        cerr << "Error: " << strerror(errno);
        return;
    } else {
        cout << "success" << endl;
    }

    // holds each line of the file
    std::string line; 
    
    // buffer to hold the 3 coordinates of each vertex read
    float raw[3];   
    int i = 0;
    while (!in.eof()) {
        i ++;

        // reads string into line variable
        getline(in, line);  

        // gets an input string stream from string object
        istringstream iss(line.c_str()); 
        
        // used to discard unused parts of the file
        char trash;
        
        if (line.compare(0, 2, "v ") == 0) {    // if current line details vertex info
            // discards the "v " part of the line
            iss >> trash;   

            // fill the raw array with vertex information
            for (int i=0; i<3; i++) iss >> raw[i];  

            // creates a 3D vector from the info and pushes it to the array
            Vector3f v = Vector3f(raw[0], raw[1], raw[2]);
            verts.push_back(v);
        } else if (line.compare(0, 2, "f ") == 0) { // if current line details face info

            // 3D vector to hold 3 indexes, 1 for each vertex of the face
            std::vector<int> f;

            int itrash; // disposal for normal index and texture index
            int idx; // buffer for vertex index

            // discard the first "f" char
            iss >> trash;
            while (iss >> idx >> trash >> itrash >> trash >> itrash) {
                // since index is 1-indexed, decrement by 1
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
