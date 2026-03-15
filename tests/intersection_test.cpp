#include <geometry.hpp>
#include <gtest/gtest.h>
#include <intersections.hpp>

namespace geometry::intersections {

bool PointsNear(const Point2D &p1, const Point2D &p2, double epsilon = 1e-9) {
    return std::abs(p1.x - p2.x) < epsilon && std::abs(p1.y - p2.y) < epsilon;
}

TEST(IntersectionVisitorTest, LineLineIntersection) {
    Line line1(Point2D{0, 0}, Point2D{2, 2});
    Line line2(Point2D{0, 2}, Point2D{2, 0});

    IntersectionVisitor visitor;
    auto result{visitor(line1, line2)};

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result.value().has_value());
    EXPECT_TRUE(PointsNear(result.value().value(), Point2D{1, 1}));
}

TEST(IntersectionVisitorTest, LineLineNoIntersection) {
    Line line1(Point2D{0, 0}, Point2D{1, 1});
    Line line2(Point2D{2, 2}, Point2D{3, 3});

    IntersectionVisitor visitor;
    auto result{visitor(line1, line2)};

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value().has_value());  // Нет пересечения
}

TEST(IntersectionVisitorTest, LineCircleIntersection) {
    Line line(Point2D{-2, 0}, Point2D{2, 0});
    Circle circle(Point2D{0, 0}, 1.0);

    IntersectionVisitor visitor;
    auto result{visitor(line, circle)};

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result.value().has_value());

    EXPECT_TRUE(PointsNear(result.value().value(), Point2D{-1, 0}));
}

TEST(IntersectionVisitorTest, LineCircleNoIntersection) {
    Line line(Point2D{2, 2}, Point2D{3, 3});
    Circle circle(Point2D{0, 0}, 1.0);

    IntersectionVisitor visitor;
    auto result{visitor(line, circle)};

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value().has_value());  // Нет пересечения
}

TEST(IntersectionVisitorTest, CircleCircleIntersection) {
    Circle circle1(Point2D{0, 0}, 2.0);
    Circle circle2(Point2D{2, 0}, 2.0);

    IntersectionVisitor visitor;
    auto result{visitor(circle1, circle2)};

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result.value().has_value());

    Point2D expected1{1, std::sqrt(3)};
    Point2D expected2{1, -std::sqrt(3)};

    bool matches = PointsNear(result.value().value(), expected1) || PointsNear(result.value().value(), expected2);
    EXPECT_TRUE(matches);
}

TEST(IntersectionVisitorTest, CircleCircleNoIntersection) {
    Circle circle1(Point2D{0, 0}, 1.0);
    Circle circle2(Point2D{5, 0}, 1.0);

    IntersectionVisitor visitor;
    auto result{visitor(circle1, circle2)};

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value().has_value());
}

TEST(IntersectionVisitorTest, UnsupportedCombination) {
    Triangle triangle(Point2D{0, 0}, Point2D{1, 0}, Point2D{0, 1});
    Rectangle rect(Point2D{2, 2}, 1, 1);

    IntersectionVisitor visitor;
    auto result{visitor(triangle, rect)};

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().message, "Unsupported shape combination for intersection");
}

TEST(GetIntersectPointTest, LineLineViaGetIntersectPoint) {
    Shape shape1 = Line(Point2D{0, 0}, Point2D{2, 2});
    Shape shape2 = Line(Point2D{0, 2}, Point2D{2, 0});

    auto result{GetIntersectPoint(shape1, shape2)};

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result.value().has_value());
    EXPECT_TRUE(PointsNear(result.value().value(), Point2D{1, 1}));
}

TEST(GetIntersectPointTest, UnsupportedCombinationViaGetIntersectPoint) {
    Shape shape1 = Triangle(Point2D{0, 0}, Point2D{1, 0}, Point2D{0, 1});
    Shape shape2 = Rectangle(Point2D{2, 2}, 1, 1);

    auto result{GetIntersectPoint(shape1, shape2)};

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().message, "Unsupported shape combination for intersection");
}

TEST(GetIntersectPointTest, CircleCircleViaGetIntersectPoint) {
    Shape shape1 = Circle(Point2D{0, 0}, 2.0);
    Shape shape2 = Circle(Point2D{2, 0}, 2.0);

    auto result{GetIntersectPoint(shape1, shape2)};

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result.value().has_value());

    Point2D expected1{1, std::sqrt(3)};
    Point2D expected2{1, -std::sqrt(3)};

    bool matches = PointsNear(result.value().value(), expected1) || PointsNear(result.value().value(), expected2);
    EXPECT_TRUE(matches);
}

}  // namespace geometry::intersections
