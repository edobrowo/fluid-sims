#pragma once

#include "util/common.hpp"
#include "util/format.hpp"

enum class Label {
    Empty = 0,
    Fluid,
    Solid
};

class LabelGrid {
public:
    LabelGrid(const i32 nx, const i32 ny);

    /// @brief Retrives the label at cell indices (i, j).
    Label operator()(const i32 i, const i32 j) const;

    /// @brief Sets the label at cell indices (i, j).
    void set(const i32 i, const i32 j, const Label label);

    /// @brief Indicates whether the specified cell is an Empty cell.
    bool isEmpty(const i32 i, const i32 j) const;

    /// @brief Indicates whether the specified cell is a Fluid cell.
    bool isFluid(const i32 i, const i32 j) const;

    /// @brief Indicates whether the specified cell is a Solid cell.
    bool isSolid(const i32 i, const i32 j) const;

    /// @brief Number of columns in the grid.
    i32 nx() const;

    /// @brief Number of rows in the grid.
    i32 ny() const;

    /// @brief Number of cells in the grid. Equal to nx() * ny().
    i32 cellCount() const;

    /// @brief Retrieve a pointer to the internal buffer.
    Label* data();

    /// @brief Fill the grid with the specified label.
    void fill(const Label label);

    /// @brief Sets the edge cells to Label::Solid.
    void setSolidBorder();

private:
    /// @brief Number of columns in the grid.
    i32 mNx;

    /// @brief Number of rows in the grid.
    i32 mNy;

    /// @brief Grid labels.
    Label* mData;
};

template <>
struct FormatWriter<Label> {
    static void write(const Label& label, StringBuffer& sb) {
        switch (label) {
        case Label::Empty:
            sb.putSafe('O');
            return;
        case Label::Fluid:
            sb.putSafe('F');
            return;
        case Label::Solid:
            sb.putSafe('S');
            return;
        default:
            unreachable;
        }
    }
};

template <>
struct FormatWriter<LabelGrid> {
    static void write(const LabelGrid& labels, StringBuffer& sb) {
        sb.putSafe('[');
        for (Index j = 0; j < labels.ny(); ++j) {
            for (Index i = 0; i < labels.nx(); ++i) {
                FormatWriter<Label>::write(labels(i, j), sb);
                if (j * labels.nx() + i < labels.cellCount() - 1)
                    sb.putSafe(',');
            }
            if (j < labels.ny() - 1)
                sb.putSafe('\n');
            else
                sb.putSafe(']');
        }
    }
};
