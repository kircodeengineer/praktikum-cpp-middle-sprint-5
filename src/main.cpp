#include <convex_hull.hpp>
#include <geometry.hpp>
#include <intersections.hpp>
#include <queries.hpp>
#include <shape_utils.hpp>
#include <triangulation.hpp>
#include <visualization.hpp>

#include <algorithm>
#include <numeric>
#include <print>
#include <ranges>

using namespace geometry;

namespace rng = std::ranges;
namespace views = std::ranges::views;

template <class... Ts>
struct Multilambda : Ts... {
    using Ts::operator()...;
};

std::string GetShapeName(const Shape &shape) {
    return std::visit(Multilambda{[](const Line &) -> std::string { return "Line"; },
                                  [](const Triangle &) -> std::string { return "Triangle"; },
                                  [](const Rectangle &) -> std::string { return "Rectangle"; },
                                  [](const RegularPolygon &) -> std::string { return "RegularPolygon"; },
                                  [](const Circle &) -> std::string { return "Circle"; },
                                  [](const Polygon &) -> std::string { return "Polygon"; },
                                  [](const auto &) -> std::string { return "Unknown Shape"; }},
                      shape);
};

void PrintAllIntersections(const Shape &shape, std::span<const Shape> others) {
    std::println("\n=== Intersections ===");

    auto process_intersection = [&shape](const Shape &other) {
        auto result{geometry::intersections::GetIntersectPoint(shape, other)};

        std::string shape_name{GetShapeName(shape)};
        std::string other_name{GetShapeName(other)};

        auto processed_result =
            result
                .and_then([&shape_name, &other_name](const std::optional<Point2D> &intersection_opt)
                              -> std::expected<bool, geometry::intersections::UnsupportedCombinationError> {
                    if (intersection_opt.has_value()) {
                        const auto &point = intersection_opt.value();
                        std::println("Пересечение найдено в точке ({:.6f}, {:.6f}) между {} и {}", point.x, point.y,
                                     shape_name, other_name);
                        return true;
                    } else {
                        std::println("Фигуры {} и {} не пересекаются", shape_name, other_name);
                        return false;
                    }
                })
                .or_else([&shape_name, &other_name](const geometry::intersections::UnsupportedCombinationError &error)
                             -> std::expected<bool, geometry::intersections::UnsupportedCombinationError> {
                    std::println("Ошибка при поиске пересечения между {} и {}: {}", shape_name, other_name,
                                 error.message);
                    return std::unexpected(error);
                });

        (void)processed_result;
    };

    std::ranges::for_each(others, process_intersection);
}

void PrintDistancesFromPointToShapes(Point2D p, std::span<const Shape> shapes) {
    std::println("\n=== Distance from Point Test ===");
    auto selected_shapes{shapes | std::views::take(5)};
    auto indexed_shapes{std::views::zip(std::views::iota(0U, shapes.size()), selected_shapes)};
    std::ranges::for_each(indexed_shapes, [&p](const auto &pair) {
        const auto &[index, shape] = pair;
        std::println("{}. Расстояние от точки {} до фигуры {} равно {}", index, p, GetShapeName(shape),
                     geometry::queries::DistanceToPoint(shape, p));
    });
}

void PerformShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Analysis ===");
    {
        std::println("\t=== Все пересечения между фигурами ===");
        auto all_intersections{geometry::utils::FindAllCollisions(shapes)};
        auto indexed_collided_shapes{std::views::zip(std::views::iota(0U, shapes.size()), all_intersections)};
        std::ranges::for_each(indexed_collided_shapes, [](const auto &pair) {
            const auto &[index, collided_shapes] = pair;
            std::println("\t{}. Пересечение между фигурой {} и {} ", index, GetShapeName(collided_shapes.first),
                         GetShapeName(collided_shapes.second));
        });
    }
    {
        auto heighest_shape_index{geometry::utils::FindHighestShape(shapes)};
        heighest_shape_index
            .transform([&shapes](auto index) {
                std::println("\t=== Самая высокая фигура ===");
                std::println("\t{}", GetShapeName(shapes[index]));
                return index;
            })
            .or_else([]() {
                std::println("\t=== Самая высокая фигура отсутствует ===");
                return std::optional<std::size_t>{};
            });
    }
    {
        std::println("\t=== Расстояние между двумя фигурами ===");
        if (shapes.size() < 2) {
            std::println("Недостаточно фигур для анализа — требуется минимум 2 фигуры");
            return;
        }

        auto all_pairs{std::views::iota(0U, shapes.size()) | std::views::transform([&](std::size_t i) {
                           return std::views::iota(i + 1U, shapes.size()) |
                                  std::views::transform([i](std::size_t j) { return std::pair{i, j}; });
                       }) |
                       std::views::join};

        std::optional<std::tuple<std::size_t, std::size_t, double>> result;
        auto distance_pair = std::ranges::find_if(all_pairs, [&shapes, &result](const auto &pair) {
            auto [i, j] = pair;
            auto distance_opt = geometry::queries::DistanceBetweenShapes(shapes[i], shapes[j]);
            if (distance_opt.has_value())
                result = std::optional<std::tuple<std::size_t, std::size_t, double>>{
                    std::make_tuple(i, j, distance_opt.value())};
            return distance_opt.has_value();
        });

        if (!result) {
            std::println("\tНе удалось найти ни одной пары фигур с вычислимым расстоянием");
            return;
        }

        auto [i, j, distance] = result.value();
        const auto &shape1{shapes[i]};
        const auto &shape2{shapes[j]};

        std::println("\tРасстояние между фигурой {} и фигурой {} равно {}", GetShapeName(shape1), GetShapeName(shape2),
                     distance);
    }
}

void PerformExtraShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Extra Analysis ===");
    auto all_indexes{std::views::iota(0U, shapes.size())};
    {
        std::println("\t=== Три фигуры выше 50 ===");

        auto index_shape_max_heights =
            all_indexes | std::views::transform([&shapes](const auto &index) {
                return std::pair<std::size_t, double>(index, geometry::queries::GetHeight(shapes[index]));
            }) |
            std::views::filter([](const auto &index_height) {
                static const double MAX_HEIGHT{50.0};
                return index_height.second > MAX_HEIGHT;
            }) |
            std::views::take(3) | std::ranges::to<std::vector>();
        std::ranges::for_each(index_shape_max_heights, [&shapes](const auto &index_shape_max_height) {
            auto [index, height] = index_shape_max_height;
            std::println("\t{}. Фигура {} высота {}", index, GetShapeName(shapes[index]), height);
        });
    }
    {
        std::println("\t=== Фигуры с максимальной и минимальной высотами ===");
        if (shapes.empty())
            return;
        auto minmax{std::ranges::minmax_element(shapes, {},
                                                [](const auto &shape) { return geometry::queries::GetHeight(shape); })};

        auto [min_it, max_it] = minmax;
        auto min_index{std::distance(shapes.begin(), min_it)};
        auto max_index{std::distance(shapes.begin(), max_it)};

        std::println("\t{}. Фигура {} — минимальная высота {}", min_index, GetShapeName(*min_it),
                     geometry::queries::GetHeight(*min_it));
        std::println("\t{}. Фигура {} — максимальная высота {}", max_index, GetShapeName(*max_it),
                     geometry::queries::GetHeight(*max_it));
    }
}

void PrintShapesHeight(std::span<const Shape> shapes) {
    std::println("\n=== Shape Height ===");
    auto indexed_shapes{std::views::zip(std::views::iota(0U, shapes.size()), shapes)};

    std::ranges::for_each(indexed_shapes, [](const auto &pair) {
        const auto &[index, shape] = pair;
        std::println("{}. {} {}", index, GetShapeName(shape), geometry::queries::GetHeight(shape));
    });
}

int main() {

    std::vector<Shape> shapes =
        utils::ParseShapes("circle 0 0 1.5; line 4 0 0 4; line 0 0 4 4; polygon 0 0 2 5; triangle 0 0 1 0 "
                           "0.5 1; polygon 0 0 1 2; badshape; circle 0 0 -1; line 0 0 0 51; line 0 0 0 52; line 0 "
                           "0 0 53; line 0 0 0 54");
    std::println("Parsed {} shapes", shapes.size());

    // Выведите индекс каждой фигуры и её высоту
    PrintShapesHeight(shapes);
    // Вызываем разработанные функции
    //
    PrintAllIntersections(shapes[0], shapes);

    PrintDistancesFromPointToShapes(Point2D{10.0, 10.0}, shapes);

    PerformShapeAnalysis(shapes);

    PerformExtraShapeAnalysis(shapes);

    //
    // Рисуем все фигуры
    //
    // Важно: после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 2ой график
    std::vector<Shape> shapes_to_draw = utils::ParseShapes("circle 0 0 1.5; line 1 2 3 4; polygon 0 0 2 5; triangle 0 "
                                                           "0 1 0 0.5 1; polygon 0 0 1 2; badshape; circle 0 0 -1");
    geometry::visualization::Draw(shapes_to_draw);

    //
    // Формируем список из вершин всех фигур

    std::vector<Point2D> points;

    std::ranges::for_each(shapes_to_draw, [&points](const auto &shape) {
        shape.visit([&points](const auto &s) { std::ranges::copy(s.Vertices(), std::back_inserter(points)); });
    });

    auto convex_hull{geometry::convex_hull::GrahamScan(points)};
    auto draw_convex_hull =
        convex_hull
            .transform([](auto &points) { return points | std::views::drop(1) | std::ranges::to<std::vector>(); })
            .transform([](auto points) -> geometry::Polygon { return geometry::Polygon{std::move(points)}; })
            .and_then([&shapes_to_draw](geometry::Polygon polygon) {
                shapes_to_draw.emplace_back(std::move(polygon));
                geometry::visualization::Draw(shapes_to_draw);
                return std::expected<std::vector<Point2D>, geometry::convex_hull::Error>();
            })
            .or_else([](const auto &error) -> std::expected<std::vector<Point2D>, geometry::convex_hull::Error> {
                std::println("{}", error.message);
                return std::unexpected(error);
            });
    (void)draw_convex_hull;

    //
    // после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 3ий график
    //

    {
        std::vector<Point2D> points = {{0, 0}, {10, 0}, {5, 8}, {15, 5}, {2, 12}};
        auto triangulation_result{geometry::triangulation::DelaunayTriangulation(points)};
        if (triangulation_result.has_value()) {
            geometry::visualization::Draw(triangulation_result.value());
        } else
            std::println("{}", triangulation_result.error().message);
    }
    return 0;
}