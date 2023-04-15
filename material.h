#ifndef MATERIAL_H
#define MATERIAL_H

#include "ray.h"
#include "hittable.h"
#include "vec3.h"
#include <iostream>

using namespace std;

struct hit_record;

class material {
public:
    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, pointLight light
    ) const = 0;

    virtual color colorR() { return color(1, 0, 0); }

};

class diffuse : public material {
public:
    diffuse(const color& a) : albedo(a) {}


    virtual color colorR() { return albedo; }

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, pointLight light
    ) const override {
        //vec3 scatter_direction = rec.normal * random_in_unit_sphere();
        vec3 scatter_direction = rec.normal + vec3::random_unit_vector();
        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

public:
    color albedo;
};

class metal : public material {
public:
    metal(const color& a) : albedo(a) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, pointLight light
    ) const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

public:
    color albedo;
};


#endif