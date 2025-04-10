#include "label_grid.hpp"

#include <algorithm>

LabelGrid::LabelGrid(const i32 nx, const i32 ny) : mNx(nx), mNy(ny) {
    assertm(mNy > 0, "number of rows must be positive");
    assertm(mNx > 0, "number of cols must be positive");

    mData = new Label[mNx * mNy];
}

LabelGrid::LabelGrid(const LabelGrid& other)
    : mNx(other.mNx), mNy(other.mNy), mData(new Label[other.mNx * other.mNy]) {
    std::copy(other.mData, other.mData + (mNx * mNy), mData);
}

LabelGrid& LabelGrid::operator=(const LabelGrid& other) {
    mNx = other.mNx;
    mNy = other.mNy;

    delete[] mData;
    mData = new Label[mNx * mNy];
    std::copy(other.mData, other.mData + (mNx * mNy), mData);

    return *this;
}

Label LabelGrid::operator()(const i32 i, const i32 j) const {
    if (0 <= i && i < mNx && 0 <= j && j < mNy) {
        return mData[j * mNx + i];
    } else {
        return Label::Solid;
    }
}

void LabelGrid::set(const i32 i, const i32 j, const Label label) {
    assertm(i >= 0, "i out of bounds");
    assertm(i < mNx, "i out of bounds");
    assertm(j >= 0, "j out of bounds");
    assertm(j < mNy, "j out of bounds");

    mData[j * mNx + i] = label;
}

bool LabelGrid::isEmpty(const i32 i, const i32 j) const {
    return (*this)(i, j) == Label::Empty;
}

bool LabelGrid::isFluid(const i32 i, const i32 j) const {
    return (*this)(i, j) == Label::Fluid;
}

bool LabelGrid::isExtrapolated(const i32 i, const i32 j) const {
    return (*this)(i, j) == Label::Extrapolated;
}

bool LabelGrid::isSolid(const i32 i, const i32 j) const {
    return (*this)(i, j) == Label::Solid;
}

bool LabelGrid::isNearFluid(const i32 i, const i32 j) const {
    return isFluid(i, j) || isExtrapolated(i, j);
}

i32 LabelGrid::nx() const {
    return mNx;
}

i32 LabelGrid::ny() const {
    return mNy;
}

i32 LabelGrid::cellCount() const {
    return mNx * mNy;
}

Label* LabelGrid::data() {
    return mData;
}

void LabelGrid::fill(const Label label) {
    std::fill_n(mData, mNy * mNx, label);
}

void LabelGrid::setSolidBorder() {
    for (i32 j = 0; j < mNy; ++j) {
        set(0, j, Label::Solid);
        set(mNx - 1, j, Label::Solid);
    }

    for (i32 i = 0; i < mNx; ++i) {
        set(i, 0, Label::Solid);
        set(i, mNy - 1, Label::Solid);
    }
}

void LabelGrid::reset() {
    for (i32 j = 0; j < mNy; ++j) {
        for (i32 i = 0; i < mNx; ++i) {
            if (isNearFluid(i, j))
                set(i, j, Label::Empty);
        }
    }
}
