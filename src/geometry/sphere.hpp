#pragma once

#include "math/common.hpp"
#include "math/vector.hpp"
#include "mesh.hpp"
#include "util/common.hpp"

namespace geometry {

class Sphere : public Mesh {
public:
    Sphere(const u32 rows, const u32 cols) {
        const u32 row_size = cols + 1;

        // Construct a quad for every rows-column pair.
        for (u32 y = 0; y <= rows; ++y) {
            for (u32 x = 0; x <= cols; ++x) {
                // Determine the x angle. Ranges between [0, 2 * Pi] (i.e.,
                // forming a ring around the sphere).
                const float u = static_cast<f32>(x) / static_cast<f32>(cols);
                const f32 x_angle = 2.0f * math::Pi * u;

                // Determine the y angle. Ranges between [0, Pi] (i.e., from the
                // bottom point of the sphere to the top point).
                const float v = static_cast<f32>(y) / static_cast<f32>(rows);
                const f32 y_angle = math::Pi * v;

                // Determine the position of this quad.
                const Vector3F position(std::cos(x_angle) * std::sin(y_angle),
                                        std::cos(y_angle),
                                        std::sin(x_angle) * std::sin(y_angle));

                // Add the position and scale it down by 2. This way the size
                // of the sphere will be normalized between [-0.5, 0.5].
                mVertices.push_back({position / 2.0f, {u, v}});

                // Add the other points and specify the triangles for the quad.
                if (y < rows && x < cols) {
                    const u32 a = y * row_size + x;
                    const u32 b = (y + 1) * row_size + x;
                    const u32 c = y * row_size + (x + 1);
                    const u32 d = (y + 1) * row_size + (x + 1);

                    mIndices.push_back(static_cast<Index>(a));
                    mIndices.push_back(static_cast<Index>(d));
                    mIndices.push_back(static_cast<Index>(b));

                    mIndices.push_back(static_cast<Index>(a));
                    mIndices.push_back(static_cast<Index>(c));
                    mIndices.push_back(static_cast<Index>(d));
                }
            }
        }
    }

    ~Sphere() = default;
};

}
