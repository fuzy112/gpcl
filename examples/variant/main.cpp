#define _USE_MATH_DEFINES
#include <gpcl/variant.hpp>
#include <vector>
#include <cmath>
#include <iostream>


using namespace gpcl;

struct rectangle_t
{
  double width_, height_;
  double area() const { return width_ * height_; }
};

struct circle_t
{
  double radius_;
  double area() const { return M_PI * radius_ * radius_; }
};

double total_area(std::vector<variant<rectangle_t, circle_t>> const &v)
{
  double s = 0.0;

  for (auto const &x : v)
  {
    s += visit([](auto const &y) { return y.area(); }, x);
  }

  return s;
}

int main()
{
  std::vector<variant<rectangle_t, circle_t>> v;

  v.push_back(circle_t{1.0});
  v.push_back(rectangle_t{2.0, 3.0});

  std::cout << "Total area: " << total_area(v) << std::endl;
}
