#pragma once

#include "matrix.hpp"
#include "util/common.hpp"
#include "vector.hpp"

Matrix4D scale(const Vector3D& s);
Matrix4D scale(const f32 s);

Matrix4D translate(const Vector3D& t);
Matrix4D translate(const f32 t);

Matrix4D rotate(const f32 angle, const Vector3D& k);
Matrix4D rotateX(const f32 angle);
Matrix4D rotateY(const f32 angle);
Matrix4D rotateZ(const f32 angle);

Matrix4D perspective_fovy(const f32 fovy,
                          const f32 aspect,
                          const f32 n,
                          const f32 f);
Matrix4D orthographic(const f32 l,
                      const f32 r,
                      const f32 b,
                      const f32 t,
                      const f32 n,
                      const f32 f);

Matrix4D lookat(const Vector3D& eye,
                const Vector3D& target,
                const Vector3D& up);
