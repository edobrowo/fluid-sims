#pragma once

#include "util/common.hpp"
#include "util/format.hpp"

template <std::floating_point T, u32 dim>
class Vector {
public:
    Vector();
    explicit Vector(const T value);
    Vector(const T c1, const T c2);
    Vector(const T c1, const T c2, const T c3);
    Vector(const T c1, const T c2, const T c3, const T c4);

    ~Vector() = default;

    const T* data() const;

    Vector(const Vector<T, dim>& other);
    Vector<T, dim>& operator=(const Vector<T, dim>& other);

    T operator[](const Index i) const;
    T& operator[](const Index i);

    Vector<T, dim>& operator+=(const Vector<T, dim>& rhs);
    Vector<T, dim>& operator-=(const Vector<T, dim>& rhs);
    Vector<T, dim>& operator*=(const T rhs);
    Vector<T, dim>& operator*=(const Vector<T, dim>& rhs);
    Vector<T, dim>& operator/=(const T rhs);
    Vector<T, dim>& operator/=(const Vector<T, dim>& rhs);

    bool isZero() const;

    T lengthSqr() const;
    T length() const;

    Vector<T, dim> normalized() const;

private:
    T mComponents[dim];
};

using Vector2D = Vector<f32, 2>;
using Vector3D = Vector<f32, 3>;
using Vector4D = Vector<f32, 4>;

using Vector2F = Vector<f32, 2>;
using Vector3F = Vector<f32, 3>;
using Vector4F = Vector<f32, 4>;

template <std::floating_point T, u32 dim>
Vector<T, dim>::Vector() : mComponents{} {
}

template <std::floating_point T, u32 dim>
Vector<T, dim>::Vector(T value) {
    for (Index i = 0; i < dim; ++i) mComponents[i] = value;
}

template <std::floating_point T, u32 dim>
Vector<T, dim>::Vector(const T c1, const T c2) {
    assert(dim == 2);
    mComponents[0] = c1;
    mComponents[1] = c2;
}

template <std::floating_point T, u32 dim>
Vector<T, dim>::Vector(const T c1, const T c2, const T c3) {
    assert(dim == 3);
    mComponents[0] = c1;
    mComponents[1] = c2;
    mComponents[2] = c3;
}

template <std::floating_point T, u32 dim>
Vector<T, dim>::Vector(const T c1, const T c2, const T c3, const T c4) {
    assert(dim == 4);
    mComponents[0] = c1;
    mComponents[1] = c2;
    mComponents[2] = c3;
    mComponents[3] = c4;
}

template <std::floating_point T, u32 dim>
const T* Vector<T, dim>::data() const {
    return mComponents;
}

template <std::floating_point T, u32 dim>
Vector<T, dim>::Vector(const Vector<T, dim>& other) {
    ::memcpy(mComponents, other.mComponents, dim * sizeof(T));
}

template <std::floating_point T, u32 dim>
Vector<T, dim>& Vector<T, dim>::operator=(const Vector<T, dim>& other) {
    ::memcpy(mComponents, other.mComponents, dim * sizeof(T));
    return *this;
}

template <std::floating_point T, u32 dim>
T Vector<T, dim>::operator[](const Index i) const {
    assert(i < dim);
    return mComponents[i];
}

template <std::floating_point T, u32 dim>
T& Vector<T, dim>::operator[](const Index i) {
    assert(i < dim);
    return mComponents[i];
}

template <std::floating_point T, u32 dim>
Vector<T, dim>& Vector<T, dim>::operator+=(const Vector<T, dim>& rhs) {
    for (Index i = 0; i < dim; ++i) mComponents[i] += rhs.mComponents[i];
    return *this;
}

template <std::floating_point T, u32 dim>
Vector<T, dim>& Vector<T, dim>::operator-=(const Vector<T, dim>& rhs) {
    for (Index i = 0; i < dim; ++i) mComponents[i] -= rhs.mComponents[i];
    return *this;
}

template <std::floating_point T, u32 dim>
Vector<T, dim>& Vector<T, dim>::operator*=(const T rhs) {
    for (Index i = 0; i < dim; ++i) mComponents[i] *= rhs;
    return *this;
}

template <std::floating_point T, u32 dim>
Vector<T, dim>& Vector<T, dim>::operator*=(const Vector<T, dim>& rhs) {
    for (Index i = 0; i < dim; ++i) mComponents[i] *= rhs.mComponents[i];
    return *this;
}

template <std::floating_point T, u32 dim>
Vector<T, dim>& Vector<T, dim>::operator/=(const T rhs) {
    for (Index i = 0; i < dim; ++i) mComponents[i] /= rhs;
    return *this;
}

template <std::floating_point T, u32 dim>
Vector<T, dim>& Vector<T, dim>::operator/=(const Vector<T, dim>& rhs) {
    for (Index i = 0; i < dim; ++i) mComponents[i] /= rhs.mComponents[i];
    return *this;
}

template <std::floating_point T, u32 dim>
static Vector<T, dim> operator-(const Vector<T, dim>& rhs) {
    Vector<T, dim> result;
    for (Index i = 0; i < dim; ++i) result[i] = -rhs[i];
    return result;
}

template <std::floating_point T, u32 dim>
static Vector<T, dim> operator+(const Vector<T, dim>& lhs,
                                const Vector<T, dim>& rhs) {
    Vector<T, dim> result;
    for (Index i = 0; i < dim; ++i) result[i] = lhs[i] + rhs[i];
    return result;
}

template <std::floating_point T, u32 dim>
static Vector<T, dim> operator-(const Vector<T, dim>& lhs,
                                const Vector<T, dim>& rhs) {
    Vector<T, dim> result;
    for (Index i = 0; i < dim; ++i) result[i] = lhs[i] - rhs[i];
    return result;
}

template <std::floating_point T, u32 dim>
static Vector<T, dim> operator*(const Vector<T, dim>& lhs, const T& rhs) {
    Vector<T, dim> result;
    for (Index i = 0; i < dim; ++i) result[i] = lhs[i] * rhs;
    return result;
}

template <std::floating_point T, u32 dim>
static Vector<T, dim> operator*(const T& lhs, const Vector<T, dim>& rhs) {
    Vector<T, dim> result;
    for (Index i = 0; i < dim; ++i) result[i] = lhs * rhs[i];
    return result;
}

template <std::floating_point T, u32 dim>
static Vector<T, dim> operator/(const Vector<T, dim>& lhs, const T& rhs) {
    Vector<T, dim> result;
    for (Index i = 0; i < dim; ++i) result[i] = lhs[i] / rhs;
    return result;
}

template <std::floating_point T, u32 dim>
static Vector<T, dim> operator/(const T& lhs, const Vector<T, dim>& rhs) {
    Vector<T, dim> result;
    for (Index i = 0; i < dim; ++i) result[i] = lhs / rhs[i];
    return result;
}

template <std::floating_point T, u32 dim>
static Vector<T, dim> operator*(const Vector<T, dim>& lhs,
                                const Vector<T, dim>& rhs) {
    Vector<T, dim> result;
    for (Index i = 0; i < dim; ++i) result[i] = lhs[i] * rhs[i];
    return result;
}

template <std::floating_point T, u32 dim>
static Vector<T, dim> operator/(const Vector<T, dim>& lhs,
                                const Vector<T, dim>& rhs) {
    Vector<T, dim> result;
    for (Index i = 0; i < dim; ++i) result[i] = lhs[i] / rhs[i];
    return result;
}

template <std::floating_point T, u32 dim>
static bool operator==(const Vector<T, dim>& lhs, const Vector<T, dim>& rhs) {
    for (Index i = 0; i < dim; ++i)
        if (lhs[i] != rhs[i])
            return false;
    return true;
}

template <std::floating_point T, u32 dim>
static bool operator!=(const Vector<T, dim>& lhs, const Vector<T, dim>& rhs) {
    return !(lhs == rhs);
}

template <std::floating_point T, u32 dim>
bool Vector<T, dim>::isZero() const {
    return *this == Vector<T, dim>();
}

template <std::floating_point T, u32 dim>
T dot(Vector<T, dim> lhs, Vector<T, dim> rhs) {
    T result = 0.0;
    for (Index i = 0; i < dim; ++i) result += lhs[i] * rhs[i];
    return result;
}

template <std::floating_point T, u32 dim>
T Vector<T, dim>::lengthSqr() const {
    return dot(*this, *this);
}

template <std::floating_point T, u32 dim>
T Vector<T, dim>::length() const {
    return ::sqrt(lengthSqr());
}

template <std::floating_point T, u32 dim>
Vector<T, dim> Vector<T, dim>::normalized() const {
    return *this / length();
}

template <std::floating_point T>
Vector<T, 3> cross(Vector<T, 3> lhs, Vector<T, 3> rhs) {
    Vector<T, 3> result;
    result[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1];
    result[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2];
    result[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0];
    return result;
}

template <std::floating_point T>
    requires FormatWritable<T>
struct FormatWriter<Vector<T, 2>> {
    static void write(const Vector<T, 2>& value, StringBuffer& sb) {
        format(sb, "({},{})", value[0], value[1]);
    }
};

template <std::floating_point T>
    requires FormatWritable<T>
struct FormatWriter<Vector<T, 3>> {
    static void write(const Vector<T, 3>& value, StringBuffer& sb) {
        format(sb, "({},{},{})", value[0], value[1], value[2]);
    }
};

template <std::floating_point T>
    requires FormatWritable<T>
struct FormatWriter<Vector<T, 4>> {
    static void write(const Vector<T, 4>& value, StringBuffer& sb) {
        format(sb, "({},{},{},{})", value[0], value[1], value[2], value[3]);
    }
};

template <std::floating_point T, u32 dim>
    requires FormatWritable<T>
struct FormatWriter<Vector<T, dim>> {
    static void write(const Vector<T, dim>& value, StringBuffer& sb) {
        sb.append("(");
        for (Index i = 0; i < dim - 1; ++i) sb.appendFormat("{},", value[i]);
        sb.appendFormat("{}", value[dim - 1]);
        sb.append(")");
    }
};
