#include <gtest/gtest.h>
#include <shape_utils.hpp>

namespace geometry::utils {
TEST(FindAllCollisionsTest, EmptyShapes) {
    std::vector<Shape> shapes;
    auto result = FindAllCollisions(std::span{shapes});
    EXPECT_TRUE(result.empty());
}

TEST(FindAllCollisionsTest, SingleShape) {
    std::vector<Shape> shapes = {Line{Point2D{0, 0}, Point2D{1, 1}}};
    auto result = FindAllCollisions(std::span{shapes});
    EXPECT_TRUE(result.empty());
}

TEST(FindAllCollisionsTest, NoCollisions) {
    std::vector<Shape> shapes = {Rectangle{Point2D{0, 0}, 1.0, 1.0}, Rectangle{Point2D{10, 10}, 1.0, 1.0}};
    auto result = FindAllCollisions(std::span{shapes});
    EXPECT_TRUE(result.empty());
}

TEST(FindAllCollisionsTest, OneCollision) {
    Rectangle rect1{Point2D{0, 0}, 2.0, 2.0};
    Rectangle rect2{Point2D{1, 1}, 2.0, 2.0};  // Пересекается с rect1

    std::vector<Shape> shapes = {rect1, rect2};
    auto result = FindAllCollisions(std::span{shapes});

    ASSERT_EQ(result.size(), 1);
    // Проверяем, что пара содержит оба прямоугольника (порядок может быть любым)
    bool found = false;
    for (const auto &pair : result) {
        if ((std::holds_alternative<Rectangle>(pair.first) && std::holds_alternative<Rectangle>(pair.second))) {
            const auto &r1 = std::get<Rectangle>(pair.first);
            const auto &r2 = std::get<Rectangle>(pair.second);
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
    Circle circle1{Point2D{0, 0}, 2.0};  // Центр (0,0), радиус 2
    Circle circle2{Point2D{3, 0}, 2.0};  // Пересекается с circle1
    Circle circle3{Point2D{6, 0}, 2.0};  // Не пересекается с circle1, но пересекается с circle2

    std::vector<Shape> shapes = {circle1, circle2, circle3};
    auto result = FindAllCollisions(std::span{shapes});

    EXPECT_EQ(result.size(), 2);  // (circle1,circle2) и (circle2,circle3)

    int count12 = 0, count23 = 0;
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
    auto result = FindHighestShape(std::span{shapes});
    EXPECT_FALSE(result.has_value());
}

TEST(FindHighestShapeTest, SingleShape) {
    Line line{Point2D{0, 0}, Point2D{1, 5}};
    std::vector<Shape> shapes = {line};
    auto result = FindHighestShape(std::span{shapes});
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0);
}

TEST(FindHighestShapeTest, HighestIsFirst) {
    Triangle tri{Point2D{0, 0}, Point2D{2, 2}, Point2D{4, 3}};
    Circle circle{Point2D{0, 0}, 1.5};

    std::vector<Shape> shapes = {tri, circle};
    auto result = FindHighestShape(std::span{shapes});
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0);
}

TEST(FindHighestShapeTest, HighestIsLast) {
    Rectangle rect{Point2D{0, 0}, 2.0, 3.0};
    RegularPolygon poly{Point2D{0, 0}, 4.0, 6};

    std::vector<Shape> shapes = {rect, poly};
    auto result = FindHighestShape(std::span{shapes});
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 1);
}

TEST(FindHighestShapeTest, AllSameHeight) {
    Circle c1{Point2D{0, 0}, 2.0};
    Circle c2{Point2D{5, 5}, 2.0};
    Circle c3{Point2D{10, 10}, 2.0};

    std::vector<Shape> shapes = {c1, c2, c3};
    auto result = FindHighestShape(std::span{shapes});
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 2);
}

TEST(FindHighestShapeTest, MixedTypes) {
    Line line{Point2D{0, 0}, Point2D{1, 1}};
    Triangle tri{Point2D{0, 0}, Point2D{1, 2}, Point2D{2, 1}};
    Polygon poly{{Point2D{0, 0}, Point2D{1, 3}, Point2D{2, 2}}};

    std::vector<Shape> shapes = {line, tri, poly};
    auto result = FindHighestShape(std::span{shapes});
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 2);
}

}  // namespace geometry::utils