#pragma once

#include "arithmetic.hpp"
#include "util/common.hpp"
#include "util/format.hpp"

template <Numeric T>
class VectorX {
public:
    explicit VectorX(const Size size);
    explicit VectorX(const Size size, const T value);

    ~VectorX();

    const T* data() const;
    const Size size() const;

    void resize(const Size size);

    VectorX(const VectorX<T>& other);
    VectorX<T>& operator=(const VectorX<T>& other);

    T operator[](const Index i) const;
    T& operator[](const Index i);

    VectorX<T>& operator+=(const VectorX<T>& rhs);
    VectorX<T>& operator-=(const VectorX<T>& rhs);
    VectorX<T>& operator*=(const T rhs);
    VectorX<T>& operator*=(const VectorX<T>& rhs);
    VectorX<T>& operator/=(const T rhs);
    VectorX<T>& operator/=(const VectorX<T>& rhs);

    bool isZero() const;

    T lengthSqr() const;
    T length() const;

    T infinityNorm() const;

    void fill(const T value);

private:
    Size mSize;
    T* mComponents;
};

using VectorXF = VectorX<f32>;
using VectorXD = VectorX<f64>;

template <Numeric T>
VectorX<T>::VectorX(const Size size) : mSize(size), mComponents(new T[mSize]) {
    assertm(size > 0, "size must be positive");
    std::fill_n(mComponents, mSize, T(0));
}

template <Numeric T>
VectorX<T>::VectorX(const Size size, const T value)
    : mSize(size), mComponents(new T[mSize]) {
    assertm(size > 0, "size must be positive");
    for (Index i = 0; i < mSize; ++i) mComponents[i] = value;
}

template <Numeric T>
VectorX<T>::~VectorX() {
    delete[] mComponents;
}

template <Numeric T>
const T* VectorX<T>::data() const {
    return mComponents;
}

template <Numeric T>
const Size VectorX<T>::size() const {
    return mSize;
}

template <Numeric T>
void VectorX<T>::resize(const Size size) {
    T* components = new T[size];

    std::fill_n(mComponents, mSize, T(0));

    for (Index i = 0; i < std::min(size, mSize); ++i)
        components[i] = mComponents[i];

    mSize = size;
    delete[] mComponents;
    mComponents = components;
}

template <Numeric T>
VectorX<T>::VectorX(const VectorX<T>& other) {
    mSize = other.mSize;
    mComponents = new T[mSize];
    ::memcpy(mComponents, other.mComponents, mSize * sizeof(T));
}

template <Numeric T>
VectorX<T>& VectorX<T>::operator=(const VectorX<T>& other) {
    delete[] mComponents;
    mSize = other.mSize;
    mComponents = new T[mSize];
    ::memcpy(mComponents, other.mComponents, mSize * sizeof(T));
    return *this;
}

template <Numeric T>
T VectorX<T>::operator[](const Index i) const {
    assertm(i < mSize, "i not less than mSize");
    return mComponents[i];
}

template <Numeric T>
T& VectorX<T>::operator[](const Index i) {
    assertm(i < mSize, "i not less than mSize");
    return mComponents[i];
}

template <Numeric T>
VectorX<T>& VectorX<T>::operator+=(const VectorX<T>& rhs) {
    assertm(mSize == rhs.mSize, "mSize must be equivalent");
    for (Index i = 0; i < mSize; ++i) mComponents[i] += rhs.mComponents[i];
    return *this;
}

template <Numeric T>
VectorX<T>& VectorX<T>::operator-=(const VectorX<T>& rhs) {
    assertm(mSize == rhs.mSize, "mSize must be equivalent");
    for (Index i = 0; i < mSize; ++i) mComponents[i] -= rhs.mComponents[i];
    return *this;
}

template <Numeric T>
VectorX<T>& VectorX<T>::operator*=(const T rhs) {
    for (Index i = 0; i < mSize; ++i) mComponents[i] *= rhs;
    return *this;
}

template <Numeric T>
VectorX<T>& VectorX<T>::operator*=(const VectorX<T>& rhs) {
    assertm(mSize == rhs.mSize, "mSize must be equivalent");
    for (Index i = 0; i < mSize; ++i) mComponents[i] *= rhs.mComponents[i];
    return *this;
}

template <Numeric T>
VectorX<T>& VectorX<T>::operator/=(const T rhs) {
    for (Index i = 0; i < mSize; ++i) mComponents[i] /= rhs;
    return *this;
}

template <Numeric T>
VectorX<T>& VectorX<T>::operator/=(const VectorX<T>& rhs) {
    assertm(mSize == rhs.mSize, "mSize must be equivalent");
    for (Index i = 0; i < mSize; ++i) mComponents[i] /= rhs.mComponents[i];
    return *this;
}

template <Numeric T>
static VectorX<T> operator-(const VectorX<T>& rhs) {
    VectorX<T> result(rhs.size());
    for (Index i = 0; i < result.size(); ++i) result[i] = -rhs[i];
    return result;
}

template <Numeric T>
static VectorX<T> operator+(const VectorX<T>& lhs, const VectorX<T>& rhs) {
    assertm(lhs.size() == rhs.size(), "mSize must be equivalent");
    VectorX<T> result(lhs.size());
    for (Index i = 0; i < result.size(); ++i) result[i] = lhs[i] + rhs[i];
    return result;
}

template <Numeric T>
static VectorX<T> operator-(const VectorX<T>& lhs, const VectorX<T>& rhs) {
    assertm(lhs.size() == rhs.size(), "mSize must be equivalent");
    VectorX<T> result(lhs.size());
    for (Index i = 0; i < result.size(); ++i) result[i] = lhs[i] - rhs[i];
    return result;
}

template <Numeric T>
static VectorX<T> operator*(const VectorX<T>& lhs, const T& rhs) {
    VectorX<T> result(lhs.size());
    for (Index i = 0; i < result.size(); ++i) result[i] = lhs[i] * rhs;
    return result;
}

template <Numeric T>
static VectorX<T> operator*(const T& lhs, const VectorX<T>& rhs) {
    VectorX<T> result(rhs.size());
    for (Index i = 0; i < result.size(); ++i) result[i] = lhs * rhs[i];
    return result;
}

template <Numeric T>
static VectorX<T> operator/(const VectorX<T>& lhs, const T& rhs) {
    VectorX<T> result(lhs.size());
    for (Index i = 0; i < result.size(); ++i) result[i] = lhs[i] / rhs;
    return result;
}

template <Numeric T>
static VectorX<T> operator/(const T& lhs, const VectorX<T>& rhs) {
    VectorX<T> result(rhs.size());
    for (Index i = 0; i < result.size(); ++i) result[i] = lhs / rhs[i];
    return result;
}

template <Numeric T>
static VectorX<T> operator*(const VectorX<T>& lhs, const VectorX<T>& rhs) {
    assertm(lhs.size() == rhs.size(), "mSize must be equivalent");
    VectorX<T> result(lhs.size());
    for (Index i = 0; i < result.size(); ++i) result[i] = lhs[i] * rhs[i];
    return result;
}

template <Numeric T>
static VectorX<T> operator/(const VectorX<T>& lhs, const VectorX<T>& rhs) {
    assertm(lhs.size() == rhs.size(), "mSize must be equivalent");
    VectorX<T> result(lhs.size());
    for (Index i = 0; i < result.size(); ++i) result[i] = lhs[i] / rhs[i];
    return result;
}

template <Numeric T>
static bool operator==(const VectorX<T>& lhs, const VectorX<T>& rhs) {
    if (lhs.size() != rhs.size())
        return false;

    for (Index i = 0; i < lhs.size(); ++i)
        if (lhs[i] != rhs[i])
            return false;

    return true;
}

template <Numeric T>
static bool operator!=(const VectorX<T>& lhs, const VectorX<T>& rhs) {
    return !(lhs == rhs);
}

template <Numeric T>
bool VectorX<T>::isZero() const {
    return *this == VectorX<T>(mSize, T(0));
}

template <Numeric T>
T dot(VectorX<T> lhs, VectorX<T> rhs) {
    assertm(lhs.size() == rhs.size(), "mSize must be equivalent");
    T result = 0.0;
    for (Index i = 0; i < lhs.size(); ++i) result += T(lhs[i]) * T(rhs[i]);
    return result;
}

template <Numeric T>
T VectorX<T>::lengthSqr() const {
    return dot(*this, *this);
}

template <Numeric T>
T VectorX<T>::length() const {
    return ::sqrt(lengthSqr());
}

template <Numeric T>
T VectorX<T>::infinityNorm() const {
    T result = T(0);
    for (Index i = 0; i < mSize; ++i)
        result = std::fmax(result, std::fabs(mComponents[i]));
    return result;
}

template <Numeric T>
void VectorX<T>::fill(const T value) {
    std::fill_n(mComponents, mSize, value);
}

template <Numeric T>
    requires FormatWritable<T>
struct FormatWriter<VectorX<T>> {
    static void write(const VectorX<T>& value, StringBuffer& sb) {
        sb.append("(");
        for (Index i = 0; i < value.size() - 1; ++i)
            sb.appendFormat("{},", value[i]);
        sb.appendFormat("{}", value[value.size() - 1]);
        sb.append(")");
    }
};
