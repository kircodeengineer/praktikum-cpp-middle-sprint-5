#include "convex_hull.hpp"
#include "geometry.hpp"
#include "intersections.hpp"
#include "queries.hpp"
#include "shape_utils.hpp"
#include "triangulation.hpp"
#include "visualization.hpp"

#include <algorithm>
#include <print>
#include <ranges>

using namespace geometry;

namespace rng = std::ranges;
namespace views = std::ranges::views;

std::string GetShapeName(const Shape &shape) {
    return std::visit(
        [](const auto &s) -> std::string {
            using T = std::decay_t<decltype(s)>;
            if constexpr (std::is_same_v<T, Line>)
                return "Line";
            else if constexpr (std::is_same_v<T, Triangle>)
                return "Triangle";
            else if constexpr (std::is_same_v<T, Rectangle>)
                return "Rectangle";
            else if constexpr (std::is_same_v<T, RegularPolygon>)
                return "RegularPolygon";
            else if constexpr (std::is_same_v<T, Circle>)
                return "Circle";
            else if constexpr (std::is_same_v<T, Polygon>)
                return "Polygon";
            else
                return "Unknown Shape";
        },
        shape);
}

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

    /*
     * Используйте ranges чтобы выбрать любые 5 фигур из списка.
     * Затем найдите расстояния от заданной точки до всех выбранных фигур.
     * Выведите результат в формате "Расстояние от точки P до фигуры S равно D"
     */
}

void PerformShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Analysis ===");

    /*
     * Используйте ranges и созданные классы чтобы:
     *     - Найти все пересечения между фигурами используя метод Bounding Box
     *     - Найти самую высокую фигуру (чья высота наибольшая)expected
     *     - Вывести расстояние между любыми двумя фигурами, которые поддерживают данную функциональность
     */
}

void PerformExtraShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Extra Analysis ===");

    /*
     * Используйте ranges и созданные классы чтобы:
     *     - Вывести 3 любые фигуры, которые находятся выше 50.0
     *     - Вывести фигуры с наименьшей и с наибольшей высотами
     */
}

int main() {
    std::vector<Shape> shapes = utils::ParseShapes("circle 0 0 1.5; line 1 2 3 4; polygon 0 0 2 5; triangle 0 0 1 0 "
                                                   "0.5 1; polygon 0 0 1 2; badshape; circle 0 0 -1");
    std::println("Parsed {} shapes", shapes.size());

    // Выведите индекс каждой фигуры и её высоту

    //
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
    //
    geometry::visualization::Draw(shapes);

    //
    // Формируем список из вершин всех фигур
    //
    std::vector<Point2D> points;

    /* ваш код здесь */

    //
    // Находим список точек, для построения выпуклой оболочки - convex hull - алгоритмом Грэхема
    // Создаём из них объект класса `Polygon` и добавляем его в список shapes
    // Рисуем все фигуры
    //

    /* ваш код здесь */

    //
    // после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 3ий график
    //

    {
        std::vector<Point2D> points = {{0, 0}, {10, 0}, {5, 8}, {15, 5}, {2, 12}};

        //
        // Используйте список точек points или свой, чтобы
        // выполнить алгоритм триангуляции Делоне алгоритмом Боуэра-Ватсона
        //
        // После успешного завершения алгоритма - выведите результат для проверки
        // используя geometry::visualization::Draw
        //
    }
    return 0;
}