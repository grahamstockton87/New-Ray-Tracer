#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"
#include "ray.h"
#include "aabb.h"


class material;

struct hit_record {
	point3 p;
	vec3 normal;
	shared_ptr<material> mat_ptr;
	double t = 0;
	bool front_face = false;


	inline void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}

};

struct pointLight {
	color col = color(0.5, 0.5, 0.5);
	vec3 position = vec3(0.0f, 1.0f, -1.0f);
	float constant = 0.0f;
	float linear = 4.0f;
	float exponent = 2.0f;
};

class hittable {
public:
	virtual bool hit(const ray& r, hit_record& rec) const = 0;

	virtual bool bounding_box(double t0, double t1, aabb& output_box) const = 0;

	virtual vec3 random(const vec3& o) const {
		return vec3(1, 0, 0);
	}

};


#endif