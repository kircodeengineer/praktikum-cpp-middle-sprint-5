#include <gtest/gtest.h>
#include <shape_utils.hpp>

namespace geometry::utils {
TEST(FindAllCollisionsTest, EmptyShapes) {
    std::vector<Shape> shapes;
    auto result{FindAllCollisions(std::span{shapes})};
    EXPECT_TRUE(result.empty());
}

TEST(FindAllCollisionsTest, SingleShape) {
    std::vector<Shape> shapes{Line{Point2D{0, 0}, Point2D{1, 1}}};
    auto result{FindAllCollisions(std::span{shapes})};
    EXPECT_TRUE(result.empty());
}

TEST(FindAllCollisionsTest, NoCollisions) {
    std::vector<Shape> shapes{Rectangle{Point2D{0, 0}, 1.0, 1.0}, Rectangle{Point2D{10, 10}, 1.0, 1.0}};
    auto result{FindAllCollisions(std::span{shapes})};
    EXPECT_TRUE(result.empty());
}

TEST(FindAllCollisionsTest, OneCollision) {
    Rectangle rect1{Point2D{0, 0}, 2.0, 2.0};
    Rectangle rect2{Point2D{1, 1}, 2.0, 2.0};

    std::vector<Shape> shapes{rect1, rect2};
    auto result{FindAllCollisions(std::span{shapes})};

    ASSERT_EQ(result.size(), 1);

    bool found{};
    for (const auto &pair : result) {
        if ((std::holds_alternative<Rectangle>(pair.first) && std::holds_alternative<Rectangle>(pair.second))) {
            const auto &r1{std::get<Rectangle>(pair.first)};
            const auto &r2{std::get<Rectangle>(pair.second)};
            if ((r1.bottom_left == Point2D{0, 0} && r2.bottom_left == Point2D{1, 1}) ||
                (r1.bottom_left == Point2D{1, 1} && r2.bottom_left == Point2D{0, 0})) {
                found = true;
                break;
            }
        }
    }
    EXPECT_TRUE(found);
}

TEST(FindAllCollisionsTest, MultipleCollisions) {
    Circle circle1{Point2D{0, 0}, 2.0};
    Circle circle2{Point2D{3, 0}, 2.0};
    Circle circle3{Point2D{6, 0}, 2.0};

    std::vector<Shape> shapes{circle1, circle2, circle3};
    auto result{FindAllCollisions(std::span{shapes})};

    EXPECT_EQ(result.size(), 2);

    int count12{}, count23{};
    for (const auto &pair : result) {
        auto is_circle1 = [](const Shape &s) {
            if (const auto *c = std::get_if<Circle>(&s))
                return c->center_p == Point2D{0, 0};
            return false;
        };
        auto is_circle2 = [](const Shape &s) {
            if (const auto *c = std::get_if<Circle>(&s))
                return c->center_p == Point2D{3, 0};
            return false;
        };
        auto is_circle3 = [](const Shape &s) {
            if (const auto *c = std::get_if<Circle>(&s))
                return c->center_p == Point2D{6, 0};
            return false;
        };

        if ((is_circle1(pair.first) && is_circle2(pair.second)) || (is_circle2(pair.first) && is_circle1(pair.second)))
            count12++;
        if ((is_circle2(pair.first) && is_circle3(pair.second)) || (is_circle3(pair.first) && is_circle2(pair.second)))
            count23++;
    }
    EXPECT_EQ(count12, 1);
    EXPECT_EQ(count23, 1);
}

TEST(FindHighestShapeTest, EmptyShapes) {
    std::vector<Shape> shapes;
    auto result{FindHighestShape(std::span{shapes})};
    EXPECT_FALSE(result.has_value());
}

TEST(FindHighestShapeTest, SingleShape) {
    Line line{Point2D{0, 0}, Point2D{1, 5}};
    std::vector<Shape> shapes{line};
    auto result{FindHighestShape(std::span{shapes})};
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0);
}

TEST(FindHighestShapeTest, HighestIsFirst) {
    Triangle tri{Point2D{0, 0}, Point2D{2, 2}, Point2D{4, 3}};
    Circle circle{Point2D{0, 0}, 1.5};

    std::vector<Shape> shapes{tri, circle};
    auto result{FindHighestShape(std::span{shapes})};
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0);
}

TEST(FindHighestShapeTest, HighestIsLast) {
    Rectangle rect{Point2D{0, 0}, 2.0, 3.0};
    RegularPolygon poly{Point2D{0, 0}, 4.0, 6};

    std::vector<Shape> shapes{rect, poly};
    auto result{FindHighestShape(std::span{shapes})};
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 1);
}

TEST(FindHighestShapeTest, AllSameHeight) {
    Circle c1{Point2D{0, 0}, 2.0};
    Circle c2{Point2D{5, 5}, 2.0};
    Circle c3{Point2D{10, 10}, 2.0};

    std::vector<Shape> shapes{c1, c2, c3};
    auto result{FindHighestShape(std::span{shapes})};
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 2);
}

TEST(FindHighestShapeTest, MixedTypes) {
    Line line{Point2D{0, 0}, Point2D{1, 1}};
    Triangle tri{Point2D{0, 0}, Point2D{1, 2}, Point2D{2, 1}};
    Polygon poly{{Point2D{0, 0}, Point2D{1, 3}, Point2D{2, 2}}};

    std::vector<Shape> shapes{line, tri, poly};
    auto result{FindHighestShape(std::span{shapes})};
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 2);
}

bool IsPoint2DEqual(const Point2D &p1, const Point2D &p2) { return p1.x == p2.x && p1.y == p2.y; }

bool IsCircleEqual(const Circle &c1, const Circle &c2) {
    return c1.center_p.x == c2.center_p.x && c1.center_p.y == c2.center_p.y && c1.radius == c2.radius;
}

bool IsLineEqual(const Line &l1, const Line &l2) {
    return IsPoint2DEqual(l1.start, l2.start) && IsPoint2DEqual(l1.end, l2.end);
}

bool IsTriangleEqual(const Triangle &t1, const Triangle &t2) {
    return IsPoint2DEqual(t1.a, t2.a) && IsPoint2DEqual(t1.b, t2.b) && IsPoint2DEqual(t1.c, t2.c);
}

bool IsRectangleEqual(const Rectangle &r1, const Rectangle &r2) {
    return IsPoint2DEqual(r1.bottom_left, r2.bottom_left) && r1.width == r2.width && r1.height == r2.height;
}

bool IsRegularPolygonEqual(const RegularPolygon &p1, const RegularPolygon &p2) {
    return IsPoint2DEqual(p1.center_p, p2.center_p) && p1.radius == p2.radius && p1.sides == p2.sides;
}

TEST(MakeCircleTest, ValidCircle) {
    auto result{MakeCircle({1.0, 2.0, 3.0})};
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(std::holds_alternative<Circle>(*result));
    auto circle{std::get<Circle>(*result)};
    EXPECT_DOUBLE_EQ(circle.center_p.x, 1.0);
    EXPECT_DOUBLE_EQ(circle.center_p.y, 2.0);
    EXPECT_DOUBLE_EQ(circle.radius, 3.0);
}

TEST(MakeCircleTest, InvalidSize) {
    auto result{MakeCircle({1.0, 2.0})};
    EXPECT_FALSE(result.has_value());
}

TEST(MakeCircleTest, NegativeRadius) {
    auto result{MakeCircle({1.0, 2.0, -1.0})};
    EXPECT_FALSE(result.has_value());
}

TEST(MakeCircleTest, ZeroRadius) {
    auto result{MakeCircle({1.0, 2.0, 0.0})};
    EXPECT_FALSE(result.has_value());
}

// Тесты для MakeLine
TEST(MakeLineTest, ValidLine) {
    auto result{MakeLine({0.0, 0.0, 1.0, 1.0})};
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(std::holds_alternative<Line>(*result));
    auto line{std::get<Line>(*result)};
    EXPECT_DOUBLE_EQ(line.start.x, 0.0);
    EXPECT_DOUBLE_EQ(line.start.y, 0.0);
    EXPECT_DOUBLE_EQ(line.end.x, 1.0);
    EXPECT_DOUBLE_EQ(line.end.y, 1.0);
}

TEST(MakeLineTest, InvalidSize) {
    auto result{MakeLine({0.0, 0.0, 1.0})};
    EXPECT_FALSE(result.has_value());
}

// Тесты для MakeTriangle
TEST(MakeTriangleTest, ValidTriangle) {
    auto result{MakeTriangle({0.0, 0.0, 1.0, 0.0, 0.0, 1.0})};
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(std::holds_alternative<Triangle>(*result));
    auto triangle{std::get<Triangle>(*result)};
    EXPECT_DOUBLE_EQ(triangle.a.x, 0.0);
    EXPECT_DOUBLE_EQ(triangle.a.y, 0.0);
    EXPECT_DOUBLE_EQ(triangle.b.x, 1.0);
    EXPECT_DOUBLE_EQ(triangle.b.y, 0.0);
    EXPECT_DOUBLE_EQ(triangle.c.x, 0.0);
    EXPECT_DOUBLE_EQ(triangle.c.y, 1.0);
}

TEST(MakeTriangleTest, InvalidSize) {
    auto result{MakeTriangle({0.0, 0.0, 1.0, 0.0})};
    EXPECT_FALSE(result.has_value());
}

// Тесты для MakeRectangle
TEST(MakeRectangleTest, ValidRectangle) {
    auto result{MakeRectangle({0.0, 0.0, 2.0, 3.0})};
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(std::holds_alternative<Rectangle>(*result));
    auto rectangle{std::get<Rectangle>(*result)};
    EXPECT_DOUBLE_EQ(rectangle.bottom_left.x, 0.0);
    EXPECT_DOUBLE_EQ(rectangle.bottom_left.y, 0.0);
    EXPECT_DOUBLE_EQ(rectangle.width, 2.0);
    EXPECT_DOUBLE_EQ(rectangle.height, 3.0);
}

TEST(MakeRectangleTest, InvalidSize) {
    auto result{MakeRectangle({0.0, 0.0, 2.0})};
    EXPECT_FALSE(result.has_value());
}

TEST(MakeRectangleTest, NegativeWidth) {
    auto result{MakeRectangle({0.0, 0.0, -2.0, 3.0})};
    EXPECT_FALSE(result.has_value());
}

TEST(MakeRectangleTest, ZeroHeight) {
    auto result{MakeRectangle({0.0, 0.0, 2.0, 0.0})};
    EXPECT_FALSE(result.has_value());
}

// Тесты для MakePolygon
TEST(MakePolygonTest, ValidPolygon) {
    auto result{MakePolygon({0.0, 0.0, 5.0, 6.0})};
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(std::holds_alternative<RegularPolygon>(*result));
    auto polygon{std::get<RegularPolygon>(*result)};
    EXPECT_DOUBLE_EQ(polygon.center_p.x, 0.0);
    EXPECT_DOUBLE_EQ(polygon.center_p.y, 0.0);
    EXPECT_DOUBLE_EQ(polygon.radius, 5.0);
    EXPECT_EQ(polygon.sides, 6);
}

TEST(MakePolygonTest, InvalidSize) {
    auto result{MakePolygon({0.0, 0.0, 5.0})};
    EXPECT_FALSE(result.has_value());
}

TEST(MakePolygonTest, NegativeRadius) {
    auto result{MakePolygon({0.0, 0.0, -5.0, 6.0})};
    EXPECT_FALSE(result.has_value());
}

TEST(MakePolygonTest, NonIntegerSides) {
    auto result{MakePolygon({0.0, 0.0, 5.0, 5.5})};
    EXPECT_FALSE(result.has_value());
}

TEST(MakePolygonTest, TooFewSides) {
    auto result{MakePolygon({0.0, 0.0, 5.0, 2.0})};
    EXPECT_FALSE(result.has_value());
}

}  // namespace geometry::utils