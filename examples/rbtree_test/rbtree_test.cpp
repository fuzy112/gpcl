#include <gpcl/lexical_cast.hpp>
#include <gpcl/rbtree.hpp>
#include <gpcl/string_utils.hpp>
#include <iostream>
#include <string>
#include <string_view>

using gpcl::rbtree;
using gpcl::rbtree_node;

using namespace std::string_literals;

class node : public rbtree_node<node>
{
  int value_;

public:
  explicit node(int v) noexcept : value_(v) {}

  int value() const noexcept { return value_; }

  bool operator<(const node &rhs) const noexcept
  {
    return value() < rhs.value();
  }
};

std::ostream &operator<<(std::ostream &os, const node &n)
{
  return os << n.value();
}

class application
{
public:
  application()
  {
    std::cin.exceptions(std::ios_base::failbit);
    std::cin.sync_with_stdio(false);
  }

  ~application()
  {
    clear();
    remove_all();
  }

  void insert(int n)
  {
    node *p = new node(n);
    tree_.insert(p);

    if (auto_display_)
      display();
  }

  void remove(int n)
  {
    node v(n);
    node *p = tree_.find(v);
    if (!p)
    {
      std::cerr << "Node " << n << " doesn't exist\n";
      return;
    }

    tree_.remove(p);
    delete p;

    if (auto_display_)
      display();
  }

  void remove_all()
  {
    while (tree_.root() != nullptr)
    {
      node *p = tree_.root();
      tree_.remove(p);
      delete p;
    }
  }

  void display() { dump_tree(tree_); }

  void auto_display(bool b) { auto_display_ = b; }

  void clear() { std::system("killall lefty"); }

  void run_one()
  {

    std::cerr << ">>> ";

    std::string line;
    std::getline(std::cin, line);

    gpcl::ssplit_results tokens;
    gpcl::split(line, tokens, " ",
                gpcl::split_flag::skip_empty_string |
                    gpcl::split_flag::trim_results);

    if (tokens.empty())
      return;

    if (tokens.str(0) == "insert")
    {
      int n = gpcl::lexical_cast<int>(tokens.str(1));
      insert(n);
    }
    else if (tokens.str(0) == "remove")
    {
      int n = gpcl::lexical_cast<int>(tokens.str(1));
      remove(n);
    }
    else if (tokens.str(0) == "remove-all")
    {
      remove_all();
    }
    else if (tokens.str(0) == "auto-display")
    {
      bool e = gpcl::lexical_cast<bool>(tokens.str(1));
      auto_display(e);
    }
    else if (tokens.str(0) == "display")
    {
      display();
    }
    else if (tokens.str(0) == "clear")
    {
      clear();
    }
    else if (tokens.str(0) == "exit")
    {
      exit(0);
    }
    else
    {
      throw std::runtime_error("Unknown command " + tokens.str(0));
    }
  }

  void run()
  {
    while (std::cin)
    {
      try
      {
        run_one();
      }
      catch (const std::exception &exc)
      {
        std::cerr << "Error: " << exc.what() << '\n';
      }
    }
  }

private:
  rbtree<node> tree_;

  bool auto_display_{false};
};

int main()
{
  application app;

  app.run();
}
