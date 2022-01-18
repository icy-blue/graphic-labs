//
// Created by icy on 2022/1/15.
//

#ifndef AABB_H
#define AABB_H


#include "rtweekend.h"
#include "hittable.h"
#include "face.h"
#include <utility>
#include <vector>
#include <cassert>

using std::vector;


class aabb : public hittable {
public:
    aabb() = default;

    aabb(const vec3 &min, const vec3 &max, shared_ptr<material> material) {
        this->_min.set(min);
        this->_max.set(max);
        mat_ptr = std::move(material);
    }

    bool hit(
            const ray &r, double t_min, double t_max, hit_record &rec) const override;

    bool getBounding(vec3 &min, vec3 &max) const override;

    HIT_TYPE getType() const override;

    void addFace(const shared_ptr<face> &face) {
        this->faces.push_back(face);
    }


public:
    vec3 _min, _max;
    vector<shared_ptr<face>> faces;
    shared_ptr<material> mat_ptr;
};

bool aabb::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    double x_min, x_max, y_min, y_max, z_min, z_max;
    x_min = y_min = z_min = infinity;
    x_max = y_max = z_max = -infinity;
    // X0+td=X1 t=(X1-X0)/d
    if (std::abs(r.direction().x()) > 1e-5) {
        x_min = (this->_min.x() - r.origin().x()) / r.direction().x();
        x_max = (this->_max.x() - r.origin().x()) / r.direction().x();
        if(x_min > x_max) std::swap(x_min, x_max);
    }
    if (std::abs(r.direction().y()) > 1e-5) {
        y_min = (this->_min.y() - r.origin().y()) / r.direction().y();
        y_max = (this->_max.y() - r.origin().y()) / r.direction().y();
        if(y_min > y_max) std::swap(y_min, y_max);
    }
    if (std::abs(r.direction().z()) > 1e-5) {
        z_min = (this->_min.z() - r.origin().z()) / r.direction().z();
        z_max = (this->_max.z() - r.origin().z()) / r.direction().z();
        if(z_min > z_max) std::swap(z_min, z_max);
    }
    if (x_max < y_min || x_max < z_min || y_max < x_min || y_max < z_min
        || z_max < x_min || z_max < y_min)
        return false;
    double t;
    if (x_min >= y_min && x_min >= z_min) {
        t = x_min;
        rec.normal.set(r.direction().x() > 0 ? -1 : 1, 0, 0);
    } else if (y_min >= x_min && y_min >= z_min) {
        t = y_min;
        rec.normal.set(0, r.direction().y() > 0 ? -1 : 1, 0);
    } else if(z_min >=x_min && z_min >= y_min){
        t = z_min;
        rec.normal.set(0, 0, r.direction().z() > 0 ? -1 : 1);
    }
    if (t < t_min || t > t_max) return false;
    rec.p = r.at(t);
    rec.t = t;
    return true;
}

bool aabb::getBounding(vec3 &min, vec3 &max) const {
    min.set(this->_min);
    max.set(this->_max);
    return true;
}

HIT_TYPE aabb::getType() const {
    return HIT_TYPE::AABB;
}

#endif //AABB_H
