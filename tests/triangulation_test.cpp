#include <gtest/gtest.h>
#include <triangulation.hpp>

namespace geometry::triangulation {
bool ApproximatelyEqual(const DelaunayTriangle &a, const DelaunayTriangle &b, double epsilon = 1e-10) {
    auto point_equal = [epsilon](const Point2D &p1, const Point2D &p2) {
        return std::abs(p1.x - p2.x) < epsilon && std::abs(p1.y - p2.y) < epsilon;
    };
    return (point_equal(a.a, b.a) && point_equal(a.b, b.b) && point_equal(a.c, b.c)) ||
           (point_equal(a.a, b.b) && point_equal(a.b, b.c) && point_equal(a.c, b.a)) ||
           (point_equal(a.a, b.c) && point_equal(a.b, b.a) && point_equal(a.c, b.b));
}

bool ContainsTriangle(const std::vector<DelaunayTriangle> &triangles, const DelaunayTriangle &target) {
    for (const auto &t : triangles) {
        if (ApproximatelyEqual(t, target))
            return true;
    }
    return false;
}

TEST(DelaunayTriangulationTest, LessThanThreePoints) {
    std::vector<Point2D> two_points{{0.0, 0.0}, {1.0, 1.0}};
    auto result{DelaunayTriangulation(two_points)};
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().message, "At least three points are required for triangulation.");
}

TEST(DelaunayTriangulationTest, ThreePointsTriangle) {
    std::vector<Point2D> points{{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};

    auto result{DelaunayTriangulation(points)};
    ASSERT_TRUE(result.has_value());
    const auto &triangles = result.value();

    EXPECT_EQ(triangles.size(), 1);

    DelaunayTriangle expected{{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};
    EXPECT_TRUE(ContainsTriangle(triangles, expected));
}

}  // namespace geometry::triangulation