#pragma once

#ifndef VEC3_H
#define VEC3_H

#include "rtweekend.h"


#include <cmath>
#include <iostream>
#include <algorithm>


using std::sqrt;

class vec3 {
public:
	vec3() : e{ 0,0,0 } {}
	vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }

	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
	double operator[](int i) const { return e[i]; }
	double& operator[](int i) { return e[i]; }

	vec3& operator+=(const vec3& v) {
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	vec3& operator*=(const double t) {
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	vec3& operator/=(const double t) {
		return *this *= 1 / t;
	}

	double length() const {
		return sqrt(length_squared());
	}

	double length_squared() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	bool equals(vec3 a) const {
		if (e[0] == a.e[0] &&
			e[1] == a.e[1] &&
			e[2] == a.e[2]) {
			return true;
		}
		else {
			return false;
		}
	}

	bool lessThan(vec3 a) const {
		if (e[0] < a.e[0] &&
			e[1] < a.e[1] &&
			e[2] < a.e[2]) {
			return true;
		}
		else {
			return false;
		}
	}

	bool greaterThan(vec3 a) const {
		if (e[0] > a.e[0] &&
			e[1] > a.e[1] &&
			e[2] > a.e[2]) {
			return true;
		}
		else {
			return false;
		}
	}


public:
	double e[3];

	inline static vec3 random() {
		return vec3(random_double(), random_double(), random_double());
	}

	inline static vec3 random(double min, double max) {
		return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}
	inline static vec3 random_unit_vector() {
		auto a = random_double(0, 2 * pi);
		auto z = random_double(-1, 1);
		auto r = sqrt(1 - z * z);
		return vec3(r * cos(a), r * sin(a), z);
	}

};

// vec3 Utility Functions




inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) {
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
	return t * v;
}

inline vec3 operator/(vec3 v, double t) {
	return (1 / t) * v;
}



inline double dot(const vec3& u, const vec3& v) {
	return u.e[0] * v.e[0]
		+ u.e[1] * v.e[1]
		+ u.e[2] * v.e[2];
}

inline vec3 normalize(const vec3 a) {
	return vec3(a / a.length());
}

inline vec3 cross(const vec3& u, const vec3& v) {
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		u.e[2] * v.e[0] - u.e[0] * v.e[2],
		u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}



inline vec3 SUB3(const vec3& v1, const vec3& v2) {

	return vec3(v1[0] - v2[0],
		v1[1] - v2[1],
		v1[2] - v2[2]);

}



inline vec3 unit_vector(vec3 v) {
	return v / v.length();
}

inline static vec3 random_in_unit_sphere() {
	while (true) {
		auto p = vec3::random(-1, 1);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}

inline static vec3 random_in_hemisphere(const vec3& normal) {
	vec3 in_unit_sphere = random_in_unit_sphere();
	if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
		return in_unit_sphere;
	else
		return -in_unit_sphere;
}

inline static vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n) * n;
}

inline vec3 random_cosine_direction() {
	auto r1 = random_double();
	auto r2 = random_double();
	auto z = sqrt(1 - r2);

	auto phi = 2 * pi * r1;
	auto x = cos(phi) * sqrt(r2);
	auto y = sin(phi) * sqrt(r2);

	return vec3(x, y, z);
}

vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
	auto cos_theta = dot(-uv, n);
	vec3 r_out_parallel = etai_over_etat * (uv + cos_theta * n);
	vec3 r_out_perp = -sqrt(1.0 - r_out_parallel.length_squared()) * n;
	return r_out_parallel + r_out_perp;
}
double schlick(double cosine, double ref_idx) {
	auto r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}
vec3 random_in_unit_disk() {
	while (true) {
		auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}

inline float cast_i2f(unsigned int i)
{
	union { float f; unsigned int i; } v;
	v.i = i;
	return v.f;
}
inline unsigned int cast_f2i(float f)
{
	union { float f; unsigned int i; } v;
	v.f = f;
	return v.i;
}

inline float xorf(float x, float y) {
	return cast_i2f(cast_f2i(x) ^ cast_f2i(y));
}

inline float signmsk(float x) {
	return cast_i2f(cast_f2i(x) & 0x80000000);
}


// Type aliases for vec3
using point3 = vec3;   // 3D point
using color = vec3;    // RGB color

#endif

