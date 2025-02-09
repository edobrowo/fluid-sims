#include "transform.hpp"

Matrix4D scale(const Vector3D& s) {
    // clang-format off
    return Matrix4D(
        {
            s[0], 0.0f, 0.0f, 0.0f,
            0.0f, s[1], 0.0f, 0.0f,
            0.0f, 0.0f, s[2], 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        }
    );
    // clang-format on
}

Matrix4D scale(const f32 s) {
    return scale(Vector3D(s));
}

Matrix4D translate(const Vector3D& t) {
    // clang-format off
    return Matrix4D(
        {
            1.0f, 0.0f, 0.0f, t[0],
            0.0f, 1.0f, 0.0f, t[1],
            0.0f, 0.0f, 1.0f, t[2],
            0.0f, 0.0f, 0.0f, 1.0f
        }
    );
    // clang-format on
}

Matrix4D translate(const f32 t) {
    return translate(Vector3D(t));
}

Matrix4D rotate(const f32 angle, const Vector3D& k) {
    // https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
    Vector3D kn = k.normalized();
    // clang-format off
    const Matrix3D K(
        {
            0.0f,  -kn[2],  kn[1],
            kn[2],  0.0f,  -kn[0],
           -kn[1],  kn[0],  0.0f,
        }
    );
    // clang-format on
    const Matrix3D I = ident<f32, 3>();
    const Matrix3D K2 = K * K;
    const Matrix3D R = I + (::sin(angle))*K + (1.0f - ::cos(angle)) * K2;
    return embed_ident<f32, 3, 4>(R, 0, 0);
}

Matrix4D rotateX(const f32 angle) {
    return rotate(angle, Vector3D(1.0f, 0.0f, 0.0f));
}

Matrix4D rotateY(const f32 angle) {
    return rotate(angle, Vector3D(0.0f, 1.0f, 0.0f));
}

Matrix4D rotateZ(const f32 angle) {
    return rotate(angle, Vector3D(0.0f, 0.0f, 1.0f));
}

Matrix4D perspective_fovy(const f32 fovy, const f32 aspect, const f32 n,
                          const f32 f) {
    const f32 h = ::tan(fovy / 2.0f);

    // clang-format off
    return Matrix4D(
        {
            1.0f / (aspect * h), 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f / h, 0.0f, 0.0f,
            0.0f, 0.0f, -(f + n) / (f - n), -(2.0f * f * n) / (f - n),
            0.0f, 0.0f, -1.0f, 0.0f
        }
    );
    // clang-format on
}

Matrix4D orthographic(const f32 l, const f32 r, const f32 b, const f32 t,
                      const f32 n, const f32 f) {
    // clang-format off
    return Matrix4D(
        {
            2.0f / (r - l), 0.0f, 0.0f, -(r + l) / (r - l),
            0.0f, 2.0f / (t - b), 0.0f, -(t + b) / (t - b),
            0.0f, 0.0f, -2.0f / (f - n), -(f + n) / (f - n),
            0.0f, 0.0f, 0.0f, 1.0f
        }
    );
    // clang-format on
}

Matrix4D lookat(const Vector3D& eye, const Vector3D& target,
                const Vector3D& up) {
    const Vector3D d((target - eye).normalized());
    const Vector3D r(cross(d, up).normalized());
    const Vector3D u(cross(r, d));

    // clang-format off
    return Matrix4D(
        {
            r[0],  r[1],  r[2], -dot(r, eye),
            u[0],  u[1],  u[2], -dot(u, eye),
           -d[0], -d[1], -d[2],  dot(d, eye),
            0.0f,  0.0f,  0.0f,  1.0f
        }
    );
    // clang-format on
}
