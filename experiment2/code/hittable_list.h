#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H
//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "rtweekend.h"
#include "face.h"
#include "hittable.h"
#include "aabb.h"

#include <memory>
#include <utility>
#include <vector>


class hittable_list : public hittable {
public:
    hittable_list() = default;

    explicit hittable_list(const shared_ptr<hittable> &object) { add(object); }

    void clear() { objects.clear(); }

    void add(const shared_ptr<hittable> &object) { objects.push_back(object); }

    bool hit(
            const ray &r, double t_min, double t_max, hit_record &rec) const override;

    bool getBounding(vec3 &min, vec3 &max) const override;

    HIT_TYPE getType() const override;

public:
    std::vector<shared_ptr<hittable>> objects;
};


bool hittable_list::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    hit_record temp_rec;
    auto hit_anything = false;
    auto closest_so_far = t_max;

    for (const auto &object: objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            if (object->getType() == HIT_TYPE::AABB) {
                aabb &node = dynamic_cast<aabb &>(*object);
                for (const auto &face: node.faces) {
                    if (face->hit(r, t_min, closest_so_far, temp_rec)) {
                        hit_anything = true;
                        closest_so_far = temp_rec.t;
                        rec = temp_rec;
                        rec.mat_ptr = node.mat_ptr;
                    }
                }
                continue;
            }
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

HIT_TYPE hittable_list::getType() const {
    return HIT_TYPE::LIST;
}

bool hittable_list::getBounding(vec3 &min, vec3 &max) const {
    return false;
}


#endif
