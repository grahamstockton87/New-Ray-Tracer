#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "aabb.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
public:
	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(shared_ptr<hittable> object) { objects.push_back(object); }


	virtual bool hit(const ray& r,hit_record& rec) const;
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;


public:
	std::vector<shared_ptr<hittable>> objects;
};

bool hittable_list::hit(const ray& r, hit_record& rec) const {
	hit_record temp_rec;
	bool hit_anything = false;

	for (const auto& object : objects) {
		if (object->hit(r, temp_rec)) {
			hit_anything = true;
			rec = temp_rec;
		}
	}

	return hit_anything;
}

bool hittable_list::bounding_box(double t0, double t1, aabb& output_box) const {
	if (objects.empty()) return false;

	aabb temp_box;
	bool first_box = true;

	for (const auto& object : objects) {
		if (!object->bounding_box(t0, t1, temp_box)) return false;
		output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
		first_box = false;
	}

	return true;
}



#endif


