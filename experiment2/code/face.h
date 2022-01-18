//
// Created by icy on 2022/1/15.
//

#ifndef FACE_H
#define FACE_H

#include "rtweekend.h"
#include "hittable.h"
#include <vector>
#include <cassert>

using std::vector;


class face : public hittable {
public:
    face() = default;

    bool hit(
            const ray &r, double t_min, double t_max, hit_record &rec) const override;

    bool getBounding(vec3 &min, vec3 &max) const override;

    HIT_TYPE getType() const override;

private:
    static bool hitTriangle(const ray &r, const vec3 &v0, const vec3 &v1, const vec3 &v2,
                            shared_ptr<double> &t, shared_ptr<double> &u, shared_ptr<double> &v);

public:
    vector<shared_ptr<point3>> points;
    shared_ptr<material> mat_ptr;

    vec3 getOutwardNormal() const {
        return unit_vector(cross(*points[1] - *points[0], *points[2] - *points[1]));
    }

    void addPoint(const shared_ptr<point3> &point) {
        this->points.push_back(point);
    }
};

bool face::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    assert(points.size() >= 3);
    bool ok = false;
    shared_ptr<double> t, u, v;
    for (int i = 1; i < points.size() - 1; i++) {
        ok = hitTriangle(r, *points[0], *points[i], *points[i + 1], t, u, v);
        if (ok) break;
    }
    // only one intersection is obtained.
    if (!ok || *t < t_min || *t > t_max) return false;
    rec.set_face_normal(r, getOutwardNormal());
    rec.t = *t;
    rec.mat_ptr = this->mat_ptr;
    rec.p = r.at(*t);
    return true;
}

bool face::getBounding(vec3 &min, vec3 &max) const {
    double x_min, y_min, z_min, x_max, y_max, z_max;
    x_min = y_min = z_min = infinity;
    x_max = y_max = z_max = -1 * infinity;
    if (this->points.empty()) return false;
    for (const auto &v: this->points) {
        x_min = std::min(x_min, v->x());
        y_min = std::min(y_min, v->y());
        z_min = std::min(z_min, v->z());
        x_max = std::max(x_max, v->x());
        y_max = std::max(y_max, v->y());
        z_max = std::max(z_max, v->z());
    }
    min.set(x_min, y_min, z_min);
    max.set(x_max, y_max, z_max);
    return true;
}

HIT_TYPE face::getType() const {
    return HIT_TYPE::FACE;
}

bool face::hitTriangle(const ray &r, const vec3 &v0, const vec3 &v1, const vec3 &v2,
                       shared_ptr<double> &t, shared_ptr<double> &u, shared_ptr<double> &v) {
    const vec3 &orig = r.origin(), &dir = r.dir;

    // E1
    vec3 E1 = v1 - v0;

    // E2
    vec3 E2 = v2 - v0;

    // P
    vec3 P = cross(dir, E2);

    // determinant - P·E1
    // 根据混合积公式的几何意义，det是E1&E2&OP组成的平行四边体的有向体积。
    double det = dot(E1, P);

    // 保证det > 0, 响应的修改T。
    vec3 T;
    if (det > 0) {
        T = orig - v0;
    } else {
        T = v0 - orig;
        det = -det;
    }

    // 如果determinant接近0，也就是有向体积接近0，就说明射线和E1&E2平面共面。
    if (det < 0.0001f) {
        return false;
    }

    u.reset(new double(dot(T, P)));
    if (*u < 0.0f || *u > det) {
        return false;
    }

    // Q
    vec3 Q = cross(T, E1);

    v.reset(new double(dot(dir, Q)));
    if (*v < 0.0f || *u + *v > det) {
        return false;
    }

    t.reset(new double(dot(E2, Q)));

    double invD = 1.0f / det;
    *t *= invD;
    *u *= invD;
    *v *= invD;

    return true;
}

#endif //FACE_H
