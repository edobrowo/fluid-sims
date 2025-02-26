#pragma once

#include "common.hpp"
#include "vector.hpp"

template <std::floating_point T, u32 p, u32 q>
class Matrix {
public:
    Matrix();
    Matrix(const T value);
    Matrix(const std::array<T, p * q>& elements);

    ~Matrix() = default;

    const T* data() const;

    T operator()(const Index row, const Index col) const;
    T& operator()(const Index row, const Index col);

    Matrix(const Matrix<T, p, q>& other);
    Matrix<T, p, q>& operator=(const Matrix<T, p, q>& other);

    Matrix<T, p, q>& operator+=(const Matrix<T, p, q>& rhs);
    Matrix<T, p, q>& operator-=(const Matrix<T, p, q>& rhs);
    Matrix<T, p, q>& operator*=(const T& rhs);
    Matrix<T, p, q>& operator*=(const Matrix<T, p, q>& rhs);
    Matrix<T, p, q>& operator/=(const T& rhs);

    bool isUpperTriangular() const;
    bool isLowerTriangular() const;
    bool isDiagonal() const;
    bool isSymmetric() const;
    bool isSkewSymmetric() const;

    std::optional<T> trace() const;

private:
    T mElements[p * q];
};

using Matrix2D = Matrix<f32, 2, 2>;
using Matrix3D = Matrix<f32, 3, 3>;
using Matrix4D = Matrix<f32, 4, 4>;

using Matrix2F = Matrix<f32, 2, 2>;
using Matrix3F = Matrix<f32, 3, 3>;
using Matrix4F = Matrix<f32, 4, 4>;

template <std::floating_point T, u32 p, u32 q>
Matrix<T, p, q>::Matrix() : mElements() {
}

template <std::floating_point T, u32 p, u32 q>
Matrix<T, p, q>::Matrix(const T value) {
    for (Index i = 0; i < p * q; ++i) mElements[i] = value;
}

template <std::floating_point T, u32 p, u32 q>
Matrix<T, p, q>::Matrix(const std::array<T, p * q>& elements) {
    ::memcpy(mElements, elements.data(), p * q * sizeof(T));
}

template <std::floating_point T, u32 p, u32 q>
const T* Matrix<T, p, q>::data() const {
    return mElements;
}

template <std::floating_point T, u32 p, u32 q>
Matrix<T, p, q>::Matrix(const Matrix<T, p, q>& other) {
    ::memcpy(mElements, other.mElements, p * q * sizeof(T));
}

template <std::floating_point T, u32 p, u32 q>
Matrix<T, p, q>& Matrix<T, p, q>::operator=(const Matrix<T, p, q>& other) {
    ::memcpy(mElements, other.mElements, p * q * sizeof(T));
    return *this;
}

template <std::floating_point T, u32 p, u32 q>
T Matrix<T, p, q>::operator()(const Index row, const Index col) const {
    return mElements[row * q + col];
}

template <std::floating_point T, u32 p, u32 q>
T& Matrix<T, p, q>::operator()(const Index row, const Index col) {
    return mElements[row * q + col];
}

template <std::floating_point T, u32 p, u32 q>
Matrix<T, p, q>& Matrix<T, p, q>::operator+=(const Matrix<T, p, q>& rhs) {
    for (Index row = 0; row < p; ++row)
        for (Index col = 0; col < q; ++col)
            mElements[row * q + col] += rhs.mElements[row * q + col];
    return *this;
}

template <std::floating_point T, u32 p, u32 q>
Matrix<T, p, q>& Matrix<T, p, q>::operator-=(const Matrix<T, p, q>& rhs) {
    for (Index row = 0; row < p; ++row)
        for (Index col = 0; col < q; ++col)
            mElements[row * q + col] -= rhs.mElements[row * q + col];
    return *this;
}

template <std::floating_point T, u32 p, u32 q>
Matrix<T, p, q>& Matrix<T, p, q>::operator*=(const T& rhs) {
    for (Index row = 0; row < p; ++row)
        for (Index col = 0; col < q; ++col) mElements[row * q + col] *= rhs;
    return *this;
}

template <std::floating_point T, u32 p, u32 q>
Matrix<T, p, q>& Matrix<T, p, q>::operator*=(const Matrix<T, p, q>& rhs) {
    assertm(p == q, "matrix dimensions do not match");
    return *this = *this * rhs;
}

template <std::floating_point T, u32 p, u32 q>
Matrix<T, p, q>& Matrix<T, p, q>::operator/=(const T& rhs) {
    for (Index row = 0; row < p; ++row)
        for (Index col = 0; col < q; ++col) mElements[row * q + col] /= rhs;
    return *this;
}

template <std::floating_point T, u32 p, u32 q>
bool Matrix<T, p, q>::isUpperTriangular() const {
    if constexpr (p != q)
        return false;
    for (Index row = 1; row < p; ++row)
        for (Index col = 0; col < row; ++col)
            if (mElements[row * q + col] != 0)
                return false;
    return true;
}

template <std::floating_point T, u32 p, u32 q>
bool Matrix<T, p, q>::isLowerTriangular() const {
    if constexpr (p != q)
        return false;
    for (Index row = 0; row < p - 1; ++row)
        for (Index col = row + 1; col < q; ++col)
            if (mElements[row * q + col] != 0)
                return false;
    return true;
}

template <std::floating_point T, u32 p, u32 q>
bool Matrix<T, p, q>::isDiagonal() const {
    if constexpr (p != q)
        return false;
    for (Index row = 0; row < p; ++row)
        for (Index col = 0; col < q; ++col)
            if (row != col && mElements[row * q + col] != 0)
                return false;
    return true;
}

template <std::floating_point T, u32 p, u32 q>
bool Matrix<T, p, q>::isSymmetric() const {
    if constexpr (p != q)
        return false;
    for (Index row = 0; row < p - 1; ++row)
        for (Index col = row + 1; col < q; ++col)
            if (mElements[row * q + col] != mElements[col * q + row])
                return false;
    return true;
}

template <std::floating_point T, u32 p, u32 q>
bool Matrix<T, p, q>::isSkewSymmetric() const {
    if constexpr (p != q)
        return false;
    for (Index row = 0; row < p - 1; ++row)
        for (Index col = row + 1; col < q; ++col)
            if (mElements[row * q + col] != -mElements[col * q + row])
                return false;
    return true;
}

template <std::floating_point T, u32 p, u32 q>
std::optional<T> Matrix<T, p, q>::trace() const {
    if constexpr (p != q)
        return std::nullopt;
    T result = 0;
    for (Index i = 0; i < p; ++i) result += mElements[i * q + i];
    return result;
}

template <std::floating_point T, u32 p, u32 q>
static Matrix<T, p, q> operator-(const Matrix<T, p, q>& rhs) {
    Matrix<T, p, q> result;
    for (Index row = 0; row < p; ++row)
        for (Index col = 0; col < q; ++col) result(row, col) = -rhs(row, col);
    return result;
}

template <std::floating_point T, u32 p, u32 q>
static Matrix<T, p, q> operator+(const Matrix<T, p, q>& lhs,
                                 const Matrix<T, p, q>& rhs) {
    Matrix<T, p, q> result;
    for (Index row = 0; row < p; ++row)
        for (Index col = 0; col < q; ++col)
            result(row, col) = lhs(row, col) + rhs(row, col);
    return result;
}

template <std::floating_point T, u32 p, u32 q>
static Matrix<T, p, q> operator-(const Matrix<T, p, q>& lhs,
                                 const Matrix<T, p, q>& rhs) {
    Matrix<T, p, q> result;
    for (Index row = 0; row < p; ++row)
        for (Index col = 0; col < q; ++col)
            result(row, col) = lhs(row, col) - rhs(row, col);
    return result;
}

template <std::floating_point T, u32 p, u32 q>
static Matrix<T, p, q> operator*(const Matrix<T, p, q>& lhs, const T rhs) {
    Matrix<T, p, q> result;
    for (Index row = 0; row < p; ++row)
        for (Index col = 0; col < q; ++col)
            result(row, col) = lhs(row, col) * rhs;
    return result;
}

template <std::floating_point T, u32 p, u32 q>
static Matrix<T, p, q> operator*(const T lhs, const Matrix<T, p, q>& rhs) {
    Matrix<T, p, q> result;
    for (Index row = 0; row < p; ++row)
        for (Index col = 0; col < q; ++col)
            result(row, col) = lhs * rhs(row, col);
    return result;
}

template <std::floating_point T, u32 p, u32 q>
static Matrix<T, p, q> operator/(const Matrix<T, p, q>& lhs, const T rhs) {
    Matrix<T, p, q> result;
    for (Index row = 0; row < p; ++row)
        for (Index col = 0; col < q; ++col)
            result(row, col) = lhs(row, col) / rhs;
    return result;
}

template <std::floating_point T, u32 p, u32 q>
static Matrix<T, p, q> operator/(const T lhs, const Matrix<T, p, q>& rhs) {
    Matrix<T, p, q> result;
    for (Index row = 0; row < p; ++row)
        for (Index col = 0; col < q; ++col)
            result(row, col) = lhs / rhs(row, col);
    return result;
}

template <std::floating_point T, u32 p, u32 q>
static Vector<T, p> operator*(const Matrix<T, p, q>& lhs,
                              const Vector<T, q> rhs) {
    Vector<T, p> result;
    for (Index row = 0; row < p; ++row)
        for (Index col = 0; col < q; ++col)
            result[row] += lhs(row, col) * rhs[col];
    return result;
}

template <std::floating_point T, u32 p, u32 q, u32 r>
static Matrix<T, p, r> operator*(const Matrix<T, p, q>& lhs,
                                 const Matrix<T, q, r>& rhs) {
    Matrix<T, p, r> result;
    for (Index row = 0; row < p; ++row)
        for (Index col = 0; col < r; ++col)
            for (Index k = 0; k < q; ++k)
                result(row, col) += lhs(row, k) * rhs(k, col);
    return result;
}

template <std::floating_point T, u32 p, u32 q>
static bool operator==(const Matrix<T, p, q>& lhs, const Matrix<T, p, q>& rhs) {
    for (Index row = 0; row < p; ++row)
        for (Index col = 0; col < q; ++col)
            if (lhs(row, col) != rhs(row, col))
                return false;
    return true;
}

template <std::floating_point T, u32 p, u32 q>
static bool operator!=(const Matrix<T, p, q>& lhs, const Matrix<T, p, q>& rhs) {
    return !(lhs == rhs);
}

template <std::floating_point T, u32 dim>
static Matrix<T, dim, dim> diag(const T value) {
    Matrix<T, dim, dim> result;
    for (Index i = 0; i < dim; ++i) result(i, i) = value;
    return result;
}

template <std::floating_point T, u32 dim>
static Matrix<T, dim, dim> ident() {
    return diag<T, dim>(1.0);
}

template <std::floating_point T, u32 p, u32 q>
static Matrix<T, q, p> transpose(const Matrix<T, p, q>& value) {
    Matrix<T, q, p> result;
    for (Index row = 0; row < q; ++row)
        for (Index col = 0; col < p; ++col) result(row, col) = value(col, row);
    return result;
}

template <std::floating_point T, u32 p1, u32 q1, u32 p2, u32 q2>
static Matrix<T, p2, q2> embed_zero(const Matrix<T, p1, q1>& value,
                                    const Index x, const Index y) {
    assertm(x < p1, "x not less than p1");
    assertm(p1 <= p2, "p1 not less than or equal to p2");
    assertm(y < q1, "y not less than q1");
    assertm(q1 <= q2, "q1 not less than or equal to q2");
    Matrix<T, p2, q2> result;
    for (Index row = x; row < p1; ++row)
        for (Index col = y; col < q1; ++col) result(row, col) = value(row, col);
    return result;
}

template <std::floating_point T, u32 dim1, u32 dim2>
static Matrix<T, dim2, dim2> embed_ident(const Matrix<T, dim1, dim1>& value,
                                         const Index x, const Index y) {
    assertm(x < dim1, "x not less than dim1");
    assertm(y < dim1, "y not less than dim1");
    assertm(dim1 <= dim2, "dim1 not less than or equal to dim2");
    Matrix<T, dim2, dim2> result = ident<T, dim2>();
    for (Index row = x; row < dim1; ++row)
        for (Index col = y; col < dim1; ++col)
            result(row, col) = value(row, col);
    return result;
}

template <std::floating_point T, u32 p, u32 q>
    requires FormatWritable<T>
struct FormatWriter<Matrix<T, p, q>> {
    static void write(const Matrix<T, p, q>& value, StringBuffer& sb) {
        sb.append("[");
        for (Index i = 0; i < p - 1; ++i) {
            sb.append("[");
            for (Index j = 0; j < q - 1; ++j)
                sb.appendFormat("{},", value(i, j));
            sb.appendFormat("{}],", value(i, q - 1));
        }
        sb.append("[");
        for (Index j = 0; j < q - 1; ++j)
            sb.appendFormat("{},", value(p - 1, j));
        sb.appendFormat("{}]]", value(p - 1, q - 1));
    }
};
