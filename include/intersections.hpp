#pragma once
#include <cmath>
#include <expected>
#include <geometry.hpp>
#include <optional>

namespace geometry::intersections {

struct UnsupportedCombinationError {
    std::string message;
    explicit UnsupportedCombinationError(std::string msg) : message(std::move(msg)) {}
};

class IntersectionVisitor {
public:
    using Result = std::expected<std::optional<Point2D>, UnsupportedCombinationError>;

    [[nodiscard]] Result operator()(const Line &line1, const Line &line2, double epsilon = 1e-9) const {
        auto a1{line1.end.y - line1.start.y};
        auto b1{line1.start.x - line1.end.x};
        auto c1{a1 * line1.start.x + b1 * line1.start.y};

        auto a2{line2.end.y - line2.start.y};
        auto b2{line2.start.x - line2.end.x};
        auto c2{a2 * line2.start.x + b2 * line2.start.y};

        auto det{a1 * b2 - a2 * b2};

        if (std::abs(det) < epsilon)
            return std::nullopt;

        auto x{(b2 * c1 - b1 * c2) / det};
        auto y{(a1 * c2 - a2 * c1) / det};

        Point2D intersection{x, y};

        auto is_on_segment = [&epsilon](const Point2D &p, const Line &l) -> bool {
            return std::abs((l.start - p).Cross(l.end - p)) < epsilon &&
                   (p.x >= std::min(l.start.x, l.end.x) - epsilon && p.x <= std::max(l.start.x, l.end.x) + epsilon) &&
                   (p.y >= std::min(l.start.y, l.end.y) - epsilon && p.y <= std::max(l.start.y, l.end.y) + epsilon);
        };

        if (is_on_segment(intersection, line1) && is_on_segment(intersection, line2))
            return intersection;

        return std::nullopt;
    }

    Result operator()(const Line &line, const Circle &circle) const {
        auto start{line.start - circle.center_p};
        auto end{line.end - circle.center_p};

        auto d{end - start};
        auto a{d.Dot(d)};
        auto b{2 * start.Dot(d)};
        auto c{start.Dot(start) - circle.radius * circle.radius};

        auto discriminant{b * b - 4 * a * c};

        if (discriminant < 0)
            return std::nullopt;

        auto sqrt_discriminant{std::sqrt(discriminant)};
        auto t1{(-b - sqrt_discriminant) / (2 * a)};
        auto t2{(-b + sqrt_discriminant) / (2 * a)};

        std::optional<Point2D> result;
        for (const auto &t : {t1, t2}) {
            if (t >= 0 && t <= 1) {
                auto point{start + d * t + circle.center_p};
                if (!result.has_value()) {
                    result = point;
                } else {
                    if ((point - line.start).Length() < (result->DistanceTo(line.start)))
                        result = point;
                }
            }
        }
        return result;
    }

    Result operator()(const Circle &circle, const Line &line) const { return (*this)(line, circle); }

    Result operator()(const Circle &circle1, const Circle &circle2) const {
        auto d{circle2.center_p - circle1.center_p};
        auto distance{d.Length()};

        // Проверяем, что окружности пересекаются
        if (distance > circle1.radius + circle2.radius || distance < std::abs(circle1.radius - circle2.radius) ||
            distance == 0)
            return std::nullopt;

        auto a{(circle1.radius * circle1.radius - circle2.radius * circle2.radius + distance * distance) /
               (2 * distance)};
        auto h{std::sqrt(circle1.radius * circle1.radius - a * a)};

        auto p2{circle1.center_p + d * (a / distance)};

        auto x3{p2.x + h * (circle2.center_p.y - circle1.center_p.y) / distance};
        auto y3{p2.y - h * (circle2.center_p.x - circle1.center_p.x) / distance};

        return Point2D{x3, y3};
    }

    template <typename T1, typename T2>
    Result operator()(const T1 &, const T2 &) const {
        UnsupportedCombinationError error{"Unsupported shape combination for intersection"};
        return std::unexpected(UnsupportedCombinationError{"Unsupported shape combination for intersection"});
    }
};

inline std::expected<std::optional<Point2D>, UnsupportedCombinationError> GetIntersectPoint(const Shape &shape1,
                                                                                            const Shape &shape2) {
    IntersectionVisitor visitor;
    return std::visit(
        [&visitor](const auto &s1, const auto &s2) -> IntersectionVisitor::Result { return visitor(s1, s2); }, shape1,
        shape2);
}
}  // namespace geometry::intersections