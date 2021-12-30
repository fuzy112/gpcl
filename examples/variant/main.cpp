#define _USE_MATH_DEFINES
#include <gpcl/variant.hpp>
#include <gpcl/vector.hpp>
#include <cmath>
#include <iostream>
using namespace gpcl;

struct Rectangle
{
  double width_, height_;
  double area() const { return width_ * height_; }
};

struct Circle
{
  double radius_;
  double area() const { return M_PI * radius_ * radius_; }
};

double total_area(gpcl::vector<variant<Rectangle, Circle>> const &v)
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
  gpcl::vector<variant<Rectangle, Circle>> v;

  v.push_back(Circle{1.0});
  v.push_back(Rectangle{2.0, 3.0});

  std::cout << "Total area: " << total_area(v) << std::endl;
}
