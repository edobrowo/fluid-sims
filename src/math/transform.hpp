#pragma once

#include "matrix.hpp"
#include "util/common.hpp"
#include "vector.hpp"

template <std::floating_point T>
Matrix<T, 4, 4> scale(const Vector<T, 3>& s) {
    // clang-format off
    return Matrix<T, 4, 4>(
        {
            s[0],    T(0.0),  T(0.0), T(0.0),
            T(0.0),  s[1],    T(0.0), T(0.0),
            T(0.0),  T(0.0),  s[2],   T(0.0),
            T(0.0),  T(0.0),  T(0.0), T(1.0)
        }
    );
    // clang-format on
}

template <std::floating_point T>
Matrix<T, 4, 4> scale(const T s) {
    return scale(Vector<T, 3>(s));
}

template <std::floating_point T>
Matrix<T, 4, 4> translate(const Vector<T, 3>& t) {
    // clang-format off
    return Matrix<T, 4, 4>(
        {
            T(1.0), T(0.0), T(0.0), t[0],
            T(0.0), T(1.0), T(0.0), t[1],
            T(0.0), T(0.0), T(1.0), t[2],
            T(0.0), T(0.0), T(0.0), T(1.0)
        }
    );
    // clang-format on
}

template <std::floating_point T>
Matrix<T, 4, 4> translate(const T t) {
    return translate(Vector<T, 3>(t));
}

template <std::floating_point T>
Matrix<T, 4, 4> rotate(const T angle, const Vector<T, 3>& k) {
    // https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula

    Vector<T, 3> kn = k.normalized();

    // clang-format off
    const Matrix<T, 3, 3> K(
        {
            T(0.0), -kn[2],   kn[1],
            kn[2],   T(0.0), -kn[0],
           -kn[1],   kn[0],   T(0.0),
        }
    );
    // clang-format on

    const Matrix<T, 3, 3> I = ident<T, 3>();
    const Matrix<T, 3, 3> K2 = K * K;
    const Matrix<T, 3, 3> R =
        I + (::sin(angle))*K + (T(1.0) - ::cos(angle)) * K2;

    return embed_ident<T, 3, 4>(R, 0, 0);
}

template <std::floating_point T>
Matrix<T, 4, 4> rotateX(const T angle) {
    return rotate(angle, Vector<T, 3>(T(1.0), T(0.0), T(0.0)));
}

template <std::floating_point T>
Matrix<T, 4, 4> rotateY(const T angle) {
    return rotate(angle, Vector<T, 3>(T(0.0), T(1.0), T(0.0)));
}

template <std::floating_point T>
Matrix<T, 4, 4> rotateZ(const T angle) {
    return rotate(angle, Vector<T, 3>(T(0.0), T(0.0), T(1.0)));
}

template <std::floating_point T>
Matrix<T, 4, 4> perspective_fovy(const T fovy,
                                 const T aspect,
                                 const T n,
                                 const T f) {
    const T h = ::tan(fovy / T(2.0));

    // clang-format off
    return Matrix<T, 4, 4>(
        {
            T(1.0) / (aspect * h), T(0.0), T(0.0), T(0.0),
            T(0.0), T(1.0) / h, T(0.0), T(0.0),
            T(0.0), T(0.0), -(f + n) / (f - n), -(T(2.0) * f * n) / (f - n),
            T(0.0), T(0.0), -T(1.0), T(0.0)
        }
    );
    // clang-format on
}

template <std::floating_point T>
Matrix<T, 4, 4> orthographic(
    const T l, const T r, const T b, const T t, const T n, const T f) {
    // clang-format off
    return Matrix<T, 4, 4>(
        {
            T(2.0) / (r - l), T(0.0), T(0.0), -(r + l) / (r - l),
            T(0.0), T(2.0) / (t - b), T(0.0), -(t + b) / (t - b),
            T(0.0), T(0.0), -T(2.0) / (f - n), -(f + n) / (f - n),
            T(0.0), T(0.0), T(0.0), T(1.0)
        }
    );
    // clang-format on
}

template <std::floating_point T>
Matrix<T, 4, 4> lookat(const Vector<T, 3>& eye,
                       const Vector<T, 3>& target,
                       const Vector<T, 3>& up) {
    const Vector<T, 3> d((target - eye).normalized());
    const Vector<T, 3> r(cross(d, up).normalized());
    const Vector<T, 3> u(cross(r, d));

    // clang-format off
    return Matrix<T, 4, 4>(
        {
            r[0],   r[1],   r[2],  -dot(r, eye),
            u[0],   u[1],   u[2],  -dot(u, eye),
           -d[0],  -d[1],  -d[2],   dot(d, eye),
            T(0.0), T(0.0), T(0.0), 1.0
        }
    );
    // clang-format on
}
