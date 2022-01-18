//
// Created by icy on 2022/1/15.
//

#ifndef OBJECTREADER_H
#define OBJECTREADER_H

#include "hittable.h"
#include "aabb.h"
#include <cstring>
#include <fstream>
#include <memory>
#include <regex>
#include <utility>

shared_ptr<aabb> readObj(const std::string &name, vec3 origin, double scale, shared_ptr<material> m) {
    shared_ptr<aabb> ans(new aabb);
    ans->mat_ptr = std::move(m);
    std::ifstream input(name.c_str());
    std::string tmp;
    std::vector<shared_ptr<vec3>> vectors;
    face f;
    while (!input.eof()) {
        std::getline(input, tmp);
        std::regex ws_re("\\s+");
        std::vector<std::string> v(
                std::sregex_token_iterator(tmp.begin(), tmp.end(), ws_re, -1),
                std::sregex_token_iterator());
        if (v[0] == "v") {
            double x, y, z;
            x = std::stod(v[1]) * scale + origin.x();
            y = std::stod(v[2]) * scale + origin.y();
            z = std::stod(v[3]) * scale + origin.z();
            vectors.push_back(std::make_shared<vec3>(x, y, z));
        } else if (v[0] == "f") {
            shared_ptr<face> face(new class face());
            for (int i = 1; i < v.size(); i++) {
                int w = std::stoi(v[i]);
                face->addPoint(vectors[w - 1]);
                f.addPoint(vectors[w - 1]);
            }
            ans->addFace(face);
        }
    }
    vec3 min, max;
    f.getBounding(min, max);
    ans->_min.set(min);
    ans->_max.set(max);
    return ans;
}

shared_ptr<aabb> readObj(const std::string &name, vec3 origin, shared_ptr<material> m) {
    return readObj(name, origin, 1, std::move(m));
}

#endif //OBJECTREADER_H
