/*
This class is based on the commonly used 3D vector class existing both in OpenGL
and in other languages such as Java.
For citation:
http://www.videotutorialsrock.com/opengl_tutorial/vec3f/text.php
*/
#ifndef _MY_3D_VECTORS_H_
#define _MY_3D_VECTORS_H_

#include <iostream>
using namespace std;

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

/* Class for 3-vectors */
class Vec3f
{
private:
	float data[3]; // !

public:
	Vec3f() { data[0] = data[1] = data[2] = 0; }

	Vec3f(const Vec3f &V) {
		data[0] = V.data[0];
		data[1] = V.data[1];
		data[2] = V.data[2]; }

	Vec3f(float d0, float d1, float d2) {
		data[0] = d0;
		data[1] = d1;
		data[2] = d2; }

	~Vec3f() { }

	float operator[](int i) const {
		assert (i >= 0 && i < 3);
		return data[i];
	}

	float x() const { return data[0]; }
	float& x()  { return data[0]; }

	float y() const { return data[1]; }
	float& y()  { return data[1]; }

	float z() const { return data[2]; }
	float& z()  { return data[2]; }

	Vec3f& operator=(const Vec3f &V) {
		data[0] = V.data[0];
		data[1] = V.data[1];
		data[2] = V.data[2];
		return *this;
	}

	int operator==(const Vec3f &V) {
		return ((data[0] == V.data[0]) &&
			(data[1] == V.data[1]) &&
			(data[2] == V.data[2]));
	}

	int operator!=(const Vec3f &V) {
		return ((data[0] != V.data[0]) ||
			(data[1] != V.data[1]) ||
			(data[2] != V.data[2]));
	}

	Vec3f& operator+=(const Vec3f &V) {
		data[0] += V.data[0];
		data[1] += V.data[1];
		data[2] += V.data[2];
		return *this;
	}

	Vec3f& operator-=(const Vec3f &V) {
		data[0] -= V.data[0];
		data[1] -= V.data[1];
		data[2] -= V.data[2];
		return *this;
	}

	Vec3f& operator*=(int i) {
		data[0] = float(data[0] * i);
		data[1] = float(data[1] * i);
		data[2] = float(data[2] * i);
		return *this;
	}

	Vec3f& operator*=(float f) {
		data[0] *= f;
		data[1] *= f;
		data[2] *= f;
		return *this;
	}

	Vec3f& operator/=(int i) {
		data[0] = float(data[0] / i);
		data[1] = float(data[1] / i);
		data[2] = float(data[2] / i);
		return *this;
	}

	Vec3f& operator/=(float f) {
		data[0] /= f;
		data[1] /= f;
		data[2] /= f;
		return *this;
	}


	friend Vec3f operator+(const Vec3f &v1, const Vec3f &v2) {
		Vec3f v3; Add(v3,v1,v2); return v3;
	}

	friend Vec3f operator-(const Vec3f &v1, const Vec3f &v2) {
		Vec3f v3; Sub(v3,v1,v2); return v3;
	}

	friend Vec3f operator*(const Vec3f &v1, float f) {
		Vec3f v2; CopyScale(v2,v1,f); return v2;
	}

	friend void Add(Vec3f &a, const Vec3f &b, const Vec3f &c ) {
		a.data[0] = b.data[0] + c.data[0];
		a.data[1] = b.data[1] + c.data[1];
		a.data[2] = b.data[2] + c.data[2];
	}

	friend void Sub(Vec3f &a, const Vec3f &b, const Vec3f &c ) {
		a.data[0] = b.data[0] - c.data[0];
		a.data[1] = b.data[1] - c.data[1];
		a.data[2] = b.data[2] - c.data[2];
	}

	friend void CopyScale(Vec3f &a, const Vec3f &b, float c ) {
		a.data[0] = b.data[0] * c;
		a.data[1] = b.data[1] * c;
		a.data[2] = b.data[2] * c;
	}


};

#endif
