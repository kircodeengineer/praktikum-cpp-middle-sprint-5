#include <geometry.hpp>
#include <gtest/gtest.h>
#include <queries.hpp>

namespace geometry::queries {
class GeometryQueriesTest : public ::testing::Test {
protected:
    Point2D point_origin{0.0, 0.0};
    Point2D point_far{10.0, 10.0};

    Line line_horizontal{Point2D{0.0, 5.0}, Point2D{10.0, 5.0}};
    Line line_vertical{Point2D{5.0, 0.0}, Point2D{5.0, 10.0}};
    Triangle triangle{Point2D{0.0, 0.0}, Point2D{10.0, 0.0}, Point2D{5.0, 8.66}};
    Rectangle rectangle{Point2D{2.0, 2.0}, 6.0, 4.0};
    Circle circle{Point2D{5.0, 5.0}, 3.0};
    Polygon polygon{std::vector<Point2D>{Point2D{1.0, 1.0}, Point2D{4.0, 1.0}, Point2D{4.0, 4.0}, Point2D{1.0, 4.0}}};
};

TEST_F(GeometryQueriesTest, DistanceToPointLine) {
    EXPECT_NEAR(5.0, DistanceToPoint(line_horizontal, point_origin), 1e-10);
    EXPECT_NEAR(0.0, DistanceToPoint(line_horizontal, Point2D{5.0, 5.0}), 1e-10);
}

TEST_F(GeometryQueriesTest, DistanceToPointTriangle) {
    EXPECT_NEAR(2.89, DistanceToPoint(triangle, Point2D{5.0, 2.89}), 1e-2);
    EXPECT_NEAR(2.82, DistanceToPoint(triangle, Point2D{-2.0, -2.0}), 1e-2);
}

TEST_F(GeometryQueriesTest, DistanceToPointRectangle) {
    EXPECT_NEAR(1.0, DistanceToPoint(rectangle, Point2D{3.0, 3.0}), 1e-10);
    EXPECT_NEAR(0.0, DistanceToPoint(rectangle, Point2D{8.0, 4.0}), 1e-10);
    EXPECT_NEAR(4.47, DistanceToPoint(rectangle, point_far), 1e-2);
}

TEST_F(GeometryQueriesTest, DistanceToPointCircle) {
    EXPECT_NEAR(0.0, DistanceToPoint(circle, Point2D{5.0, 5.0}), 1e-10);
    EXPECT_NEAR(0.0, DistanceToPoint(circle, Point2D{8.0, 5.0}), 1e-10);
    EXPECT_NEAR(2.0, DistanceToPoint(circle, Point2D{10.0, 5.0}), 1e-10);
}

TEST_F(GeometryQueriesTest, DistanceToPointPolygon) {
    EXPECT_NEAR(1.41, DistanceToPoint(polygon, Point2D{2.0, 2.0}), 1e-2);
    EXPECT_NEAR(1.41, DistanceToPoint(polygon, Point2D{5.0, 2.0}), 1e-2);
}

TEST_F(GeometryQueriesTest, GetBoundBoxLine) {
    BoundingBox bb{GetBoundBox(line_horizontal)};
    EXPECT_NEAR(0.0, bb.min_x, 1e-10);
    EXPECT_NEAR(10.0, bb.max_x, 1e-10);
    EXPECT_NEAR(5.0, bb.min_y, 1e-10);
    EXPECT_NEAR(5.0, bb.max_y, 1e-10);
}

TEST_F(GeometryQueriesTest, GetBoundBoxRectangle) {
    BoundingBox bb{GetBoundBox(rectangle)};
    EXPECT_NEAR(2.0, bb.min_x, 1e-10);
    EXPECT_NEAR(8.0, bb.max_x, 1e-10);
    EXPECT_NEAR(2.0, bb.min_y, 1e-10);
    EXPECT_NEAR(6.0, bb.max_y, 1e-10);
}

TEST_F(GeometryQueriesTest, GetBoundBoxCircle) {
    BoundingBox bb{GetBoundBox(circle)};
    EXPECT_NEAR(2.0, bb.min_x, 1e-10);
    EXPECT_NEAR(8.0, bb.max_x, 1e-10);
    EXPECT_NEAR(2.0, bb.min_y, 1e-10);
    EXPECT_NEAR(8.0, bb.max_y, 1e-10);
}

TEST_F(GeometryQueriesTest, GetBoundBoxPolygon) {
    BoundingBox bb{GetBoundBox(polygon)};
    EXPECT_NEAR(1.0, bb.min_x, 1e-10);
    EXPECT_NEAR(4.0, bb.max_x, 1e-10);
    EXPECT_NEAR(1.0, bb.min_y, 1e-10);
    EXPECT_NEAR(4.0, bb.max_y, 1e-10);
}

TEST_F(GeometryQueriesTest, GetHeightRectangle) {
    auto height{GetHeight(rectangle)};
    EXPECT_NEAR(6.0, height, 1e-10);
}

TEST_F(GeometryQueriesTest, GetHeightTriangle) {
    auto height{GetHeight(triangle)};
    EXPECT_NEAR(8.66, height, 1e-2);
}

TEST_F(GeometryQueriesTest, GetHeightCircle) {
    auto height{GetHeight(circle)};
    EXPECT_NEAR(8.0, height, 1e-10);
}

TEST_F(GeometryQueriesTest, GetHeightPolygon) {
    auto height{GetHeight(polygon)};
    EXPECT_NEAR(3.0, height, 1e-10);
}

TEST_F(GeometryQueriesTest, DistanceBetweenShapesCircles) {
    Circle circle1{Point2D{0.0, 0.0}, 2.0};
    Circle circle2{Point2D{6.0, 0.0}, 1.0};

    auto distance{DistanceBetweenShapes(circle1, circle2)};
    ASSERT_TRUE(distance.has_value());
    EXPECT_NEAR(3.0, distance.value(), 1e-10);
}

TEST_F(GeometryQueriesTest, DistanceBetweenShapesLines) {
    Line line1{Point2D{0.0, 0.0}, Point2D{2.0, 0.0}};
    Line line2{Point2D{5.0, 0.0}, Point2D{7.0, 0.0}};

    auto distance{DistanceBetweenShapes(line1, line2)};
    ASSERT_TRUE(distance.has_value());
    EXPECT_NEAR(3.0, distance.value(), 1e-10);
}

TEST_F(GeometryQueriesTest, DistanceBetweenShapesUnsupportedCombination) {
    Triangle triangle1{Point2D{0.0, 0.0}, Point2D{3.0, 0.0}, Point2D{1.5, 2.6}};
    Polygon polygon1{
        std::vector<Point2D>{Point2D{10.0, 10.0}, Point2D{12.0, 10.0}, Point2D{12.0, 12.0}, Point2D{10.0, 12.0}}};

    auto distance{DistanceBetweenShapes(triangle1, polygon1)};
    EXPECT_FALSE(distance.has_value());
}

TEST_F(GeometryQueriesTest, DistanceBetweenShapesOverlappingCircles) {
    Circle circle1{Point2D{0.0, 0.0}, 3.0};
    Circle circle2{Point2D{2.0, 0.0}, 2.0};

    auto distance{DistanceBetweenShapes(circle1, circle2)};
    ASSERT_TRUE(distance.has_value());
    EXPECT_NEAR(0.0, distance.value(), 1e-10);
}

TEST_F(GeometryQueriesTest, DistanceBetweenShapesSameShape) {
    auto distance{DistanceBetweenShapes(circle, circle)};
    ASSERT_TRUE(distance.has_value());
    EXPECT_NEAR(0.0, distance.value(), 1e-10);
}
}  // namespace geometry::queries