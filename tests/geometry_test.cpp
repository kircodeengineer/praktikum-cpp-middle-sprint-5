#include "geometry.hpp"
#include <gtest/gtest.h>
namespace geometry {

TEST(Point2DFormatterTest, Point) {
    Point2D point{1.0, 2.0};
    std::string result = std::format("{}", point);
    EXPECT_EQ(result, "(1.00, 2.00)");
}

TEST(VectorPoint2DFormatterTest, EmptyVector) {
    std::vector<Point2D> empty_vector;
    std::string result = std::format("{}", empty_vector);
    EXPECT_EQ(result, "");
}

TEST(VectorPoint2DFormatterTest, SinglePoint) {
    std::vector<Point2D> single_point{{1.0, 2.0}};
    std::string result = std::format("{}", single_point);
    EXPECT_EQ(result, "(1.00, 2.00)");
}

TEST(VectorPoint2DFormatterTest, MultiplePointsSpaceSeparated) {
    std::vector<Point2D> points{{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}};
    std::string result = std::format("{}", points);
    EXPECT_EQ(result, "(1.00, 2.00) (3.00, 4.00) (5.00, 6.00)");
}

TEST(VectorPoint2DFormatterTest, MultiplePointsNewLineSeparated) {
    std::vector<Point2D> points{{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}};
    std::string result = std::format("{:new_line}", points);
    std::string expected = "(1.00, 2.00)\n\t(3.00, 4.00)\n\t(5.00, 6.00)";
    EXPECT_EQ(result, expected);
}

TEST(VectorPoint2DFormatterTest, VectorWithNegativeCoordinates) {
    std::vector<Point2D> mixed_points{{1.1, -2.2}, {-3.3, 4.4}, {5.5, 6.6}};
    std::string result_space = std::format("{}", mixed_points);
    std::string expected_space = "(1.10, -2.20) (-3.30, 4.40) (5.50, 6.60)";
    EXPECT_EQ(result_space, expected_space);

    std::string result_newline = std::format("{:new_line}", mixed_points);
    std::string expected_newline = "(1.10, -2.20)\n\t(-3.30, 4.40)\n\t(5.50, 6.60)";
    EXPECT_EQ(result_newline, expected_newline);
}

TEST(VectorPoint2DFormatterTest, PointsWithFractionalCoordinates) {
    std::vector<Point2D> fractional_points{{1.123, 2.456}, {3.789, 4.012}};
    std::string result = std::format("{}", fractional_points);
    EXPECT_EQ(result, "(1.12, 2.46) (3.79, 4.01)");
}

TEST(VectorPoint2DFormatterTest, ParseInvalidSpecifierThrows) {
    std::formatter<std::vector<geometry::Point2D>> formatter;
    std::string_view invalid_spec{"invalid"};

    std::format_parse_context parse_ctx(invalid_spec);

    EXPECT_THROW(formatter.parse(parse_ctx), std::format_error);
}

TEST(VectorPoint2DFormatterTest, ParseValidNewLineSpecifier) {
    std::formatter<std::vector<geometry::Point2D>> formatter;
    std::string_view valid_spec{"new_line"};

    std::format_parse_context parse_ctx(valid_spec);

    auto end = formatter.parse(parse_ctx);
    EXPECT_EQ(end, valid_spec.end());
}
}  // namespace geometry