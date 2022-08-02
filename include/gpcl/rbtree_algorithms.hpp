//
// rbtree_algorithms.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_RBTREE_ALGORITHMS_HPP
#define GPCL_RBTREE_ALGORITHMS_HPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/identity.hpp>
#include <gpcl/invoke.hpp>
#include <gpcl/transparent_compare.hpp>

namespace gpcl {

template <typename NodeTraits>
struct rbtree_algorithms
{
  using node_traits = NodeTraits;
  using node_type = typename node_traits::node_type;
  using node_pointer = typename node_traits::node_pointer;
  using const_node_pointer = typename node_traits::const_node_pointer;
  using color_type = typename node_traits::color_type;

  static constexpr color_type red() noexcept { return node_traits::red(); }

  static constexpr color_type black() noexcept { return node_traits::black(); }

  static constexpr node_pointer null() noexcept { return node_pointer(); }

  static void init(node_pointer n) noexcept
  {
    node_traits::set_parent(n, null());
    node_traits::set_left(n, null());
    node_traits::set_right(n, null());
    node_traits::set_color(n, red());
  }

  static void init_header(node_pointer header) noexcept
  {
    node_traits::set_parent(header, null());
    node_traits::set_left(header, header);
    node_traits::set_right(header, header);
    // node_traits::set_color(header, red());
  }

  // Complexity: O(h) where h is height of n.
  static node_pointer get_header(const_node_pointer n) noexcept
  {
    GPCL_ASSERT(n != null());

    for (;;)
    {
      node_pointer p = node_traits::get_parent(n);
      if (node_traits::get_parent(p) == n)
        return p;
      n = p;
    }
  }

  // Complexity: O(d) where d is depth of n
  static node_pointer leftmost(node_pointer n) noexcept
  {
    node_pointer p = null();

    while (n != null())
    {
      p = n;
      n = node_traits::get_left(n);
    }

    return p;
  }

  // Complexity: O(d) where d is depth of n
  static node_pointer rightmost(node_pointer n) noexcept
  {
    node_pointer p = null();

    while (n != null())
    {
      p = n;
      n = node_traits::get_right(n);
    }

    return p;
  }

  static node_pointer begin_node(const_node_pointer header) noexcept
  {
    return node_traits::get_right(header);
  }

  static node_pointer end_node(const_node_pointer header) noexcept
  {
    return std::pointer_traits<node_pointer>::pointer_to(
        const_cast<node_type &>(*header));
  }

  static node_pointer root_node(const_node_pointer header) noexcept
  {
    return node_traits::get_parent(header);
  }

  static void swap_tree(node_pointer header1, node_pointer header2) noexcept
  {
    node_pointer t = node_traits::get_parent(header1);
    node_traits::set_parent(header1, node_traits::get_parent(header2));
    node_traits::set_parent(header2, t);

    t = node_traits::get_left(header1);
    node_traits::set_left(header1, node_traits::get_left(header2));
    node_traits::set_left(header2, t);

    t = node_traits::get_right(header1);
    node_traits::set_right(header1, node_traits::get_right(header2));
    node_traits::set_right(header2, t);
  }

  // Complexity: O(h) where h is the height of the tree.
  static node_pointer prev_node(node_pointer n) noexcept
  {
    GPCL_ASSERT(n != null());

    node_pointer t = node_traits::get_left(n);
    if (t != null())
      return rightmost(t);

    node_pointer p = node_traits::get_parent(n);
    while (node_traits::get_left(p) == n && node_traits::get_parent(p) != n)
    {
      n = p;
      p = node_traits::get_parent(p);
    }

    return p;
  }

  // Complexity: O(h) where h is the height of the tree.
  static node_pointer next_node(node_pointer n) noexcept
  {
    GPCL_ASSERT(n != null());

    node_pointer t = node_traits::get_right(n);
    if (t != null())
      return leftmost(t);

    node_pointer p = node_traits::get_parent(n);
    while (node_traits::get_right(p) == n && node_traits::get_parent(p) != n)
    {
      n = p;
      p = node_traits::get_parent(p);
    }

    return p;
  }

  // Complexity: O(1)
  static void rotate_right(node_pointer n) noexcept
  {
    /*
            n               x
          /               /   \
        x           ->   a     n
       / \                    /
      a   b                  b

    */

    GPCL_ASSERT(n != null());
    node_pointer x = node_traits::get_left(n);
    GPCL_ASSERT(x != null());

    node_pointer p = node_traits::get_parent(n);
    GPCL_ASSERT(p != null());

    node_pointer b = node_traits::get_right(x);

    if (node_traits::get_parent(p) != n)
    {
      if (node_traits::get_left(p) == n)
        node_traits::set_left(p, x);
      else
        node_traits::set_right(p, x);
    }
    else
    {
      node_traits::set_parent(p, x);
    }
    node_traits::set_parent(n, x);
    node_traits::set_parent(x, p);
    node_traits::set_right(x, n);
    node_traits::set_left(n, b);
    if (b != null())
      node_traits::set_parent(b, n);
  }

  // Complexity: O(1)
  static void rotate_left(node_pointer n) noexcept
  {
    /*
            n                 x
              \             /   \
                x      -> n       b
               / \         \
              a   b         a
    */

    GPCL_ASSERT(n != null());
    node_pointer x = node_traits::get_right(n);
    GPCL_ASSERT(x != null());

    node_pointer p = node_traits::get_parent(n);
    GPCL_ASSERT(p != null());

    node_pointer a = node_traits::get_left(x);

    if (node_traits::get_parent(p) != n)
    {
      if (node_traits::get_left(p) == n)
        node_traits::set_left(p, x);
      else
        node_traits::set_right(p, x);
    }
    else
    {
      node_traits::set_parent(p, x);
    }

    node_traits::set_parent(n, x);
    node_traits::set_parent(x, p);
    node_traits::set_left(x, n);
    node_traits::set_right(n, a);
    if (a != null())
      node_traits::set_parent(a, n);
  }

  // @param old_node the node to replace
  // @param header header node of the tree
  // @param new_node a node not in any tree
  // Complexity: O(1)
  static void replace_node(node_pointer old_node, node_pointer header,
                           node_pointer new_node) noexcept
  {
    node_pointer p = node_traits::get_parent(old_node);
    node_pointer l = node_traits::get_left(old_node);
    node_pointer r = node_traits::get_right(old_node);

    if (p != header)
    {
      if (node_traits::get_left(p) == old_node)
        node_traits::set_left(p, new_node);
      else
        node_traits::set_right(p, new_node);
    }
    else
    {
      node_traits::set_parent(p, new_node);
    }

    if (node_traits::get_left(header) == old_node)
      node_traits::set_left(header, new_node);
    if (node_traits::get_right(header) == old_node)
      node_traits::set_right(header, new_node);

    if (l != null())
      node_traits::set_parent(l, new_node);
    if (r != null())
      node_traits::set_parent(r, new_node);

    node_traits::set_parent(new_node, p);
    node_traits::set_left(new_node, l);
    node_traits::set_right(new_node, r);

    node_traits::set_color(new_node, node_traits::get_color(old_node));
  }

  // Complexity: O(log(size))
  static void rebalance_after_insert(node_pointer h, node_pointer n) noexcept
  {
    node_traits::set_color(n,
                           n == node_traits::get_parent(h) ? black() : red());

    for (;;)
    {
      node_pointer p = node_traits::get_parent(n);
      if (p == h || node_traits::get_color(p) == black())
        break;

      node_pointer g = node_traits::get_parent(p);
      GPCL_ASSERT(g != null() && g != h);
      node_pointer t = node_traits::get_right(g);

      if (p != t)
      {
        if (t != null() && node_traits::get_color(t) == red())
        {
          /*
                g
              /   \
            p       t

          */
          node_traits::set_color(p, black());
          node_traits::set_color(t, black());
          n = g;
          node_traits::set_color(n, n == node_traits::get_parent(h) ? black()
                                                                    : red());
        }
        else
        {
          if (node_traits::get_right(p) == n)
          {
            /*
                  g
                /   \
              p       t
               \
                n
            */

            using std::swap;
            swap(p, n);
            GPCL_ASSERT(node_traits::get_right(n) != null());
            rotate_left(n);
          }

          /*
                    g
                  /   \
                p       t
               /
              n
          */
          node_traits::set_color(p, black());
          node_traits::set_color(g, red());
          GPCL_ASSERT(node_traits::get_left(g) != null());
          rotate_right(g);
          break;
        }
      }
      else
      {
        t = node_traits::get_left(g);

        if (t != null() && node_traits::get_color(t) == red())
        {
          node_traits::set_color(p, black());
          node_traits::set_color(t, black());
          n = g;
          node_traits::set_color(n, n == node_traits::get_parent(h) ? black()
                                                                    : red());
        }
        else
        {
          if (node_traits::get_left(p) == n)
          {
            using std::swap;
            swap(p, n);
            GPCL_ASSERT(node_traits::get_left(n) != null());
            rotate_right(n);
          }

          node_traits::set_color(p, black());
          node_traits::set_color(g, red());
          GPCL_ASSERT(node_traits::get_right(g) != null());
          rotate_left(g);
          break;
        }
      }
    }
  }

  // Complexity: O(1)
  static void erase(node_pointer h, node_pointer z) noexcept
  {
    GPCL_ASSERT(h != null());
    GPCL_ASSERT(z != null());

    /* y is either z or z's successor and has at most one child */
    node_pointer y = (node_traits::get_left(z) != null() &&
                      node_traits::get_right(z) != null())
                         ? next_node(z)
                         : z;

    /* y's parent */
    node_pointer p = node_traits::get_parent(y);

    /* y's single child or null */
    node_pointer x = (node_traits::get_left(y) != null())
                         ? node_traits::get_left(y)
                         : node_traits::get_right(y);

    /* remove y */
    if (p != h)
    {
      if (node_traits::get_left(p) == y)
        node_traits::set_left(p, x);
      else
        node_traits::set_right(p, x);
    }
    else
    {
      node_traits::set_parent(p, x);
    }
    if (x != null())
      node_traits::set_parent(x, p);

    bool y_color = node_traits::get_color(y);

    if (y != z)
    {
      replace_node(z, h, y);
    }
    else
    {
      if (node_traits::get_left(h) == z)
        node_traits::set_left(h, x != null() ? x : p);
      if (node_traits::get_right(h) == z)
        node_traits::set_right(h, x != null() ? x : p);
    }

    if (y_color == red())
      return;

    for (;;)
    {
      if (x != null() && node_traits::get_color(x) == red())
      {
        node_traits::set_color(x, black());
        break;
      }

      if (x == node_traits::get_parent(h))
        break;

      node_pointer t = node_traits::get_right(p);

      if (t != x)
      {
        GPCL_ASSERT(t != null());
        if (node_traits::get_color(t) == red())
        {

          /*
                p                    t
              /   \        ->      /   \
            (x)     t            p       b
                   / \          / \
                  a   b       (x)  a
          */
          node_traits::set_color(p, red());
          node_traits::set_color(t, black());
          rotate_left(p);
          t = node_traits::get_right(p);
        }

        if ((node_traits::get_left(t) == null() ||
             node_traits::get_color(node_traits::get_left(t)) == black()) &&
            (node_traits::get_right(t) == null() ||
             node_traits::get_color(node_traits::get_right(t)) == black()))
        {
          node_traits::set_color(t, red());
          x = p;
          p = node_traits::get_parent(x);
        }
        else
        {
          node_pointer b = node_traits::get_right(t);

          if (b == null() || node_traits::get_color(b) != red())
          {
            b = t;

            rotate_right(t);
            node_traits::set_color(b, red());

            t = node_traits::get_parent(b);
            node_traits::set_color(t, black());
          }

          /*
                p                        t
              /   \                    /   \
            (x)     t       ->       p       b
                   / \              / \
                  a   b           (x)  a
          */
          node_traits::set_color(t, node_traits::get_color(p));
          node_traits::set_color(b, black());
          node_traits::set_color(p, black());
          rotate_left(p);
          break;
        }
      }
      else
      {
        t = node_traits::get_left(p);
        GPCL_ASSERT(t != null());

        if (node_traits::get_color(t) == red())
        {
          node_traits::set_color(p, red());
          node_traits::set_color(t, black());
          rotate_right(p);
          t = node_traits::get_left(p);
        }

        if ((node_traits::get_left(t) == null() ||
             node_traits::get_color(node_traits::get_left(t)) == black()) &&
            (node_traits::get_right(t) == null() ||
             node_traits::get_color(node_traits::get_right(t)) == black()))
        {
          node_traits::set_color(t, red());
          x = p;
          p = node_traits::get_parent(x);
        }

        else
        {
          node_pointer b = node_traits::get_left(t);

          if (b == null() || node_traits::get_color(b) != red())
          {
            b = t;

            rotate_left(t);
            node_traits::set_color(b, red());

            t = node_traits::get_parent(b);
            node_traits::set_color(t, black());
          }

          node_traits::set_color(t, node_traits::get_color(p));
          node_traits::set_color(b, black());
          node_traits::set_color(p, black());
          rotate_right(p);
          break;
        }
      }
    }
  }

  template <typename K, typename KeyNodePtrCompare,
            typename std::enable_if<
                std::is_same<K, const_node_pointer>::value ||
                    std::is_same<K, node_pointer>::value ||
                    is_transparent_compare<KeyNodePtrCompare>::value,
                int>::type = 0>
  static std::size_t count(const_node_pointer header, const K &k,
                           const KeyNodePtrCompare &comp)
  {
    std::size_t num{0};

    for (node_pointer n = lower_bound(header, k, comp),
                      end = upper_bound(header, k, comp);
         n != end; n = next_node(n))
      ++num;

    return num;
  }

  static std::size_t size(const_node_pointer header)
  {
    std::size_t num{0};

    for (node_pointer n = begin_node(header), end = end_node(header); n != end;
         n = next_node(n))
      ++num;

    return num;
  }

  template <typename K, typename KeyNodePtrCompare,
            typename std::enable_if<
                std::is_same<K, const_node_pointer>::value ||
                    std::is_same<K, node_pointer>::value ||
                    is_transparent_compare<KeyNodePtrCompare>::value,
                int>::type = 0>
  static node_pointer find(const_node_pointer header, const K &k,
                           const KeyNodePtrCompare &comp)
  {
    node_pointer q = node_traits::get_parent(header);
    node_pointer p = null();

    while (q != null())
    {
      p = q;
      if (comp(k, q))
        q = node_traits::get_left(q);
      else if (comp(q, k))
        q = node_traits::get_right(q);
      else
        return q;
    }

    return null();
  }

  template <typename K, typename KeyNodePtrCompare,
            typename std::enable_if<
                std::is_same<K, const_node_pointer>::value ||
                    std::is_same<K, node_pointer>::value ||
                    is_transparent_compare<KeyNodePtrCompare>::value,
                int>::type = 0>
  static node_pointer upper_bound(const_node_pointer header, const K &k,
                                  const KeyNodePtrCompare &comp)
  {
    node_pointer q = node_traits::get_parent(header);
    node_pointer p = end_node(header);

    while (q != null() && comp(k, q))
    {
      p = q;
      q = node_traits::get_left(q);
    }

    while (q != null() && !comp(k, q))
    {
      q = node_traits::get_right(q);
    }

    return q != null() ? q : p;
  }

  template <typename K, typename KeyNodePtrCompare,
            typename std::enable_if<
                std::is_same<K, const_node_pointer>::value ||
                    std::is_same<K, node_pointer>::value ||
                    is_transparent_compare<KeyNodePtrCompare>::value,
                int>::type = 0>
  static node_pointer lower_bound(const_node_pointer header, const K &k,
                                  const KeyNodePtrCompare &comp)
  {
    node_pointer q = node_traits::get_parent(header);
    node_pointer p = end_node(header);

    while (q != null() && !comp(q, k))
    {
      p = q;
      q = node_traits::get_left(q);
    }

    while (q != null() && comp(q, k))
    {
      q = node_traits::get_right(q);
    }

    return q != null() ? q : p;
  }

  template <typename K, typename KeyNodePtrCompare,
            typename std::enable_if<
                std::is_same<K, const_node_pointer>::value ||
                    std::is_same<K, node_pointer>::value ||
                    is_transparent_compare<KeyNodePtrCompare>::value,
                int>::type = 0>
  static std::pair<node_pointer, node_pointer>
  equal_range(const_node_pointer header, const K &k,
              const KeyNodePtrCompare &comp)
  {
    return std::make_pair(lower_bound(header, k, comp),
                          upper_bound(header, k, comp));
  }

  static void insert_before(node_pointer header, node_pointer pos,
                            node_pointer new_node) noexcept
  {
    if (pos == header)
    {
      node_pointer p = node_traits::get_left(header);
      node_traits::set_parent(new_node, p);
      node_traits::set_left(header, new_node);
      if (p != header)
        node_traits::set_right(p, new_node);
      if (node_traits::get_parent(header) == null())
      {
        node_traits::set_parent(header, new_node);
        node_traits::set_right(header, new_node);
      }
    }
    else
    {
      node_pointer t = node_traits::get_left(pos);

      if (t != null())
      {
        node_pointer p = rightmost(t);
        node_traits::set_right(p, new_node);
        node_traits::set_parent(new_node, p);
      }
      else
      {
        node_traits::set_left(pos, new_node);
        node_traits::set_parent(new_node, pos);
        if (node_traits::get_right(header) == pos)
          node_traits::set_right(header, new_node);
      }
    }

    node_traits::set_left(new_node, null());
    node_traits::set_right(new_node, null());

    rebalance_after_insert(header, new_node);
  }

  template <typename NodePtrCompare>
  static void insert_equal_upper_bound(node_pointer header,
                                       node_pointer new_node,
                                       NodePtrCompare comp)
  {
    node_pointer pos = upper_bound(header, new_node, comp);
    insert_before(header, pos, new_node);
  }
};

} // namespace gpcl

#endif // !GPCL_RBTREE_ALGORITHMS_HPP
