#include <convex_hull.hpp>
#include <gtest/gtest.h>

namespace geometry::convex_hull {

bool ApproximatelyEqual(const std::vector<Point2D> &a, const std::vector<Point2D> &b, double epsilon = 1e-10) {
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::abs(a[i].x - b[i].x) > epsilon || std::abs(a[i].y - b[i].y) > epsilon)
            return false;
    }
    return true;
}

TEST(GrahamScanTest, ThreePointsTriangle) {
    std::vector<Point2D> points{{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};

    auto result{GrahamScan(points)};
    ASSERT_TRUE(result.has_value());

    std::vector<Point2D> expected{{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};

    EXPECT_TRUE(ApproximatelyEqual(result.value(), expected));
}

TEST(GrahamScanTest, FourPointsSquare) {
    std::vector<Point2D> points{{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}};

    auto result{GrahamScan(points)};
    ASSERT_TRUE(result.has_value());

    std::vector<Point2D> expected{{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}};

    EXPECT_TRUE(ApproximatelyEqual(result.value(), expected));
}

TEST(GrahamScanTest, LessThanThreePoints) {
    std::vector<Point2D> two_points{{0.0, 0.0}, {1.0, 1.0}};
    auto result1{GrahamScan(two_points)};
    EXPECT_FALSE(result1.has_value());
    EXPECT_EQ(result1.error().message, "At least three points are required for convex hull.");

    std::vector<Point2D> one_point{{0.0, 0.0}};
    auto result2{GrahamScan(one_point)};
    EXPECT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error().message, "At least three points are required for convex hull.");
}
}  // namespace geometry::convex_hull
