#pragma once
#include "geometry.hpp"
#include <utility>
#include <vector>

namespace geometry::utils {

std::vector<Shape> ParseShapes(std::string_view input);

std::vector<std::pair<Shape, Shape>> FindAllCollisions(std::span<const Shape> shapes);

std::optional<size_t> FindHighestShape(std::span<const Shape> shapes);

std::optional<Shape> MakeCircle(const std::vector<double> &v);

std::optional<Shape> MakeLine(const std::vector<double> &v);

std::optional<Shape> MakeTriangle(const std::vector<double> &v);

std::optional<Shape> MakeRectangle(const std::vector<double> &v);

std::optional<Shape> MakePolygon(const std::vector<double> &v);
}  // namespace geometry::utils