#ifndef GPCL_DETAIL_JSON_HPP
#define GPCL_DETAIL_JSON_HPP

#include <gpcl/lexical_cast.hpp>
#include <gpcl/shared_ptr.hpp>
#include <gpcl/variant.hpp>

#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <type_traits>
#include <vector>

namespace gpcl::detail {

class bad_json_access : public std::exception
{
};

namespace json_cast_detail {

template <typename T, typename S, typename = void>
struct is_lexical_castable : std::false_type
{
};

template <typename T, typename S>
struct is_lexical_castable<
    T, S, std::void_t<decltype(gpcl::lexical_cast<T>(std::declval<S>()))>>
    : std::true_type
{
};

template <typename T, typename S,
          std::enable_if_t<is_lexical_castable<T, S>::value, int> = 0>
T json_cast(const S &s)
{
  return lexical_cast<T>(s);
}

template <typename T, typename S,
          std::enable_if_t<!is_lexical_castable<T, S>::value, int> = 0>
T json_cast(const S &s)
{
  GPCL_THROW(bad_json_access());
}

template <typename T>
struct json_cast_impl
{
  template <typename S>
  T operator()(S &&s) const
  {
    return json_cast<T>(std::forward<S>(s));
  }
};
} // namespace json_cast_detail

template <typename T>
constexpr json_cast_detail::json_cast_impl<T> json_cast{};

namespace json_size_detail {

template <typename T, typename = void>
struct is_sized : std::false_type
{
};

template <typename T>
struct is_sized<T, std::void_t<decltype(std::size(std::declval<T const &>()))>>
    : std::true_type
{
};

template <typename T, std::enable_if_t<is_sized<T>::value, int> = 0>
auto json_size(const T &x)
{
  return std::size(x);
}

template <typename T, std::enable_if_t<!is_sized<T>::value, int> = 0>
auto json_size(const T &)
{
  GPCL_THROW(bad_json_access());
  return 0;
}

struct json_size_impl
{
  template <typename T>
  std::size_t operator()(const T &x) const
  {
    return json_size(x);
  }
};

} // namespace json_size_detail

constexpr json_size_detail::json_size_impl json_size{};

namespace json_at_detail {

template <typename R, typename T, typename I, typename = R>
struct is_at_indexable : std::false_type
{
};

template <typename R, typename T, typename I>
struct is_at_indexable<R, T, I,
                       decltype(std::declval<T &>().at(std::declval<I>()))>
    : std::true_type
{
};

template <typename R, typename T, typename A1,
          std::enable_if_t<is_at_indexable<R, T, A1>::value, int> = 0>
R json_at(T &x, A1 const &a1)
{
  return x.at(a1);
}

template <typename R, typename T, typename A1,
          std::enable_if_t<!is_at_indexable<R, T, A1>::value, int> = 0>
R json_at(T &x, A1 const &a1)
{
  GPCL_THROW(bad_json_access());
}

template <typename R>
struct json_at_impl
{
  using result_type = R;

  template <typename T, typename A1>
  result_type operator()(T &x, A1 const &a1) const
  {
    return json_at<R>(x, a1);
  }

  template <typename A1>
  auto operator()(A1 const &a1) const
  {
    return [a1](auto &x) -> result_type { return json_at<R>(x, a1); };
  }
};

} // namespace json_at_detail

template <typename R>
constexpr json_at_detail::json_at_impl<R> json_at{};

namespace json_index_detail {

template <typename R, typename T, typename A1, typename = R>
struct is_indexable : std::false_type
{
};

template <typename R, typename T, typename A1>
struct is_indexable<R, T, A1, decltype(std::declval<T &>()[std::declval<A1>()])>
    : std::true_type
{
};

template <typename R, typename T, typename A1,
          std::enable_if_t<is_indexable<R, T, A1>::value, int> = 0>
R json_index(T &x, const A1 &a1)
{
  return x[a1];
}

template <typename R, typename T, typename A1,
          std::enable_if_t<!is_indexable<R, T, A1>::value, int> = 0>
R json_index(T &x, const A1 &a1)
{
  GPCL_THROW(bad_json_access());
}

template <typename R>
struct json_index_impl
{
  using result_type = R;

  template <typename T, typename A1>
  result_type operator()(T &x, A1 const &a1) const
  {
    return json_index<R>(x, a1);
  }

  template <typename A1>
  auto operator()(A1 const &a1) const
  {
    return [a1](auto &x) -> result_type { return json_index<R>(x, a1); };
  }
};

} // namespace json_index_detail

template <typename R>
constexpr json_index_detail::json_index_impl<R> json_index{};

template <
    typename IntegerType, typename FloatType,
    template <typename CharType, typename CharTraits, typename Allocator>
    class StringType,
    template <typename Key, typename Mapped, typename Allocator> class MapType,
    template <typename Value, typename Allocator> class VectorType,

    typename CharType = char, typename CharTraits = std::char_traits<CharType>,
    typename Allocator = std::allocator<char>>
struct basic_json
{
  using integer_type = IntegerType;
  using float_type = FloatType;
  using boolean_type = bool;
  using null_type = monostate;

  struct value_type;

  using allocator_type = Allocator;

  static_assert(std::is_same_v<CharType, typename CharTraits::char_type>);

  template <typename T>
  using rebind_allocator_type =
      typename std::allocator_traits<Allocator>::template rebind_alloc<T>;

  template <typename T>
  using rebind_allocator_traits =
      typename std::allocator_traits<Allocator>::template rebind_traits<T>;

  using string_type =
      StringType<CharType, CharTraits, rebind_allocator_type<CharType>>;
  using object_type =
      MapType<string_type, value_type,
              rebind_allocator_type<std::pair<const string_type, value_type>>>;
  using array_type = VectorType<value_type, rebind_allocator_type<value_type>>;

  struct null_tag : in_place_type_t<null_type>
  {
  };
  struct boolean_tag : in_place_type_t<bool>
  {
  };
  struct integer_tag : in_place_type_t<integer_type>
  {
  };
  struct float_tag : in_place_type_t<float_type>
  {
  };
  struct string_tag : in_place_type_t<string_type>
  {
  };
  struct object_tag : in_place_type_t<object_type>
  {
  };
  struct array_tag : in_place_type_t<array_type>
  {
  };

  using data_type = variant<null_type, boolean_type, integer_type, float_type,
                            string_type, shared_ptr<const string_type>,
                            object_type, shared_ptr<const object_type>,
                            array_type, shared_ptr<const array_type>>;

  struct clone_visitor
  {
    data_type &dest_;

    template <typename T>
    void operator()(const T &x) const
    {
      dest_ = x;
    }

    void operator()(string_type const &x) const
    {
      dest_ = allocate_shared<const string_type>(allocator_type(), x);
    }

    void operator()(object_type const &x) const
    {
      dest_ = allocate_shared<const object_type>(allocator_type(), x);
    }

    void operator()(array_type const &x) const
    {
      dest_ = allocate_shared<const array_type>(allocator_type(), x);
    }
  };

  template <typename F>
  struct const_visitor
  {
    F f_;

    template <typename T>
    explicit const_visitor(T &&t) : f_(std::forward<T>(t))
    {
    }

    template <typename T>
    decltype(auto) operator()(const T &x) const
    {
      return f_(x);
    }

    template <typename T>
    decltype(auto) operator()(const shared_ptr<const T> &x) const
    {
      return f_(*x);
    }
  };

  template <typename F>
  static const_visitor<std::decay_t<F>> make_const_visitor(F &&f)
  {
    return const_visitor<std::decay_t<F>>{std::forward<F>(f)};
  }

  template <typename F>
  struct cow_visitor
  {
    F f_;

    data_type &dest_;

    template <typename U>
    cow_visitor(U &&u, data_type &d) : f_(std::forward<U>(u)),
                                       dest_(d)
    {
    }

    template <typename T>
    decltype(auto) operator()(T &x) const
    {
      return f_(x);
    }

    template <typename T>
    decltype(auto) operator()(shared_ptr<const T> x) const
    {
      dest_ = *x;
      return f_(get<T>(dest_));
    }
  };

  template <typename F>
  static cow_visitor<std::decay_t<F>> make_cow_visitor(F &&f, data_type &dest)
  {
    return cow_visitor<std::decay_t<F>>{std::forward<F>(f), dest};
  }

  using ostream_type = std::basic_ostream<CharType, CharTraits>;

  using istream_type = std::basic_istream<CharType, CharTraits>;

  class serializing_visitor
  {
  private:
    ostream_type *os_;

  public:
    explicit serializing_visitor(ostream_type &os) : os_(&os) {}

    ostream_type &get_ostream() const { return *os_; }

    void operator()(null_type) const { get_ostream() << "null"; }

    void operator()(integer_type i) const { get_ostream() << i; }

    void operator()(float_type f) const
    {
      typename ostream_type::sentry s(get_ostream());
      if (!s)
        return;
      get_ostream() << std::setprecision(6) << f;
    }

    void operator()(const string_type &s) const
    {
      get_ostream() << std::quoted(s);
    }

    void operator()(const boolean_type &b) const
    {
      typename ostream_type::sentry sentry(get_ostream());
      if (!sentry)
        return;
      get_ostream() << std::boolalpha << b;
    }

    void operator()(const array_type &a) const
    {
      typename ostream_type::sentry sentry(get_ostream());
      if (!sentry)
        return;

      get_ostream() << '[';
      auto iter = a.cbegin();
      auto const end = a.cend();
      if (iter != end)
      {
        get_ostream() << *iter;

        while (++iter != end && get_ostream())
        {
          get_ostream() << ',';
          get_ostream() << *iter;
        }
      }
      get_ostream() << ']';
    }

    void operator()(const object_type &o) const
    {
      typename ostream_type::sentry sentry(get_ostream());
      if (!sentry)
        return;

      get_ostream() << '{';
      auto iter = o.cbegin();
      auto const end = o.cend();
      if (iter != end)
      {
        get_ostream() << std::quoted(iter->first) << ':';
        get_ostream() << iter->second;

        while (++iter != end && get_ostream())
        {
          get_ostream() << ',';
          get_ostream() << std::quoted(iter->first) << ':';
          get_ostream() << iter->second;
        }
      }
      get_ostream() << '}';
    }

    template <typename T>
    void operator()(const shared_ptr<T> &p) const
    {
      return operator()(*p);
    }
  };

  using iterator_data_type =
      variant<typename array_type::iterator, typename object_type::iterator>;

  using const_iterator_data_type =
      variant<typename array_type::const_iterator,
              typename object_type::const_iterator>;

  struct iterator
  {
    iterator_data_type data_;

    using iterator_category = std::common_type_t<
        typename std::iterator_traits<
            typename array_type::iterator>::iterator_category,
        typename std::iterator_traits<
            typename object_type::iterator>::iterator_category>;

    constexpr iterator() = default;

    constexpr iterator(typename array_type::iterator it) : data_(it) {}

    constexpr iterator(typename object_type::iterator it) : data_(it) {}

    // constexpr iterator(const iterator &) = default;

    // constexpr iterator &operator=(const iterator &) = default;

    constexpr iterator &operator=(typename array_type::iterator it)
    {
      data_ = it;
      return *this;
    }

    constexpr iterator &operator=(typename object_type::iterator it)
    {
      data_ = it;
      return *this;
    }

    decltype(auto) operator*() const
    {
      if (auto *p = get_if<typename array_type::iterator>(&data_))
        return **p;

      GPCL_THROW(bad_json_access());
    }

    decltype(auto) operator->() const { return std::addressof(**this); }

    decltype(auto) pair() const
    {
      if (auto *p = get_if<typename object_type::iterator>(&data_))
        return **p;

      GPCL_THROW(bad_json_access());
    }

    decltype(auto) key() const { return pair().first; }

    decltype(auto) value() const { return pair().second; }

    decltype(auto) operator++()
    {
      visit([](auto &it) { ++it; }, data_);
      return *this;
    }

    decltype(auto) operator++(int)
    {
      auto result = *this;
      ++*this;
      return result;
    }

    decltype(auto) operator--()
    {
      visit([](auto &it) { --it; }, data_);
      return *this;
    }

    decltype(auto) operator--(int)
    {
      auto result = *this;
      --*this;
      return result;
    }

    bool operator==(iterator const &rhs) const { return data_ == rhs.data_; }

    bool operator!=(iterator const &rhs) const { return data_ != rhs.data_; }

    // bool operator<(iterator const &rhs) const { return data_ < rhs.data_; }
  };

  struct const_iterator
  {
    const_iterator_data_type data_;

    using iterator_category = std::common_type_t<
        typename std::iterator_traits<
            typename array_type::const_iterator>::iterator_category,
        typename std::iterator_traits<
            typename object_type::const_iterator>::iterator_category>;

    constexpr const_iterator() = default;

    constexpr const_iterator(typename array_type::const_iterator it) : data_(it)
    {
    }

    constexpr const_iterator(typename object_type::const_iterator it)
        : data_(it)
    {
    }

    // constexpr iterator(const iterator &) = default;

    // constexpr iterator &operator=(const iterator &) = default;

    constexpr const_iterator &operator=(typename array_type::const_iterator it)
    {
      data_ = it;
      return *this;
    }

    constexpr const_iterator &operator=(typename object_type::const_iterator it)
    {
      data_ = it;
      return *this;
    }

    decltype(auto) operator*() const
    {
      if (auto *p = get_if<typename array_type::const_iterator>(&data_))
        return **p;

      GPCL_THROW(bad_json_access());
    }

    decltype(auto) operator->() const { return std::addressof(**this); }

    decltype(auto) pair() const
    {
      if (auto *p = get_if<typename object_type::const_iterator>(&data_))
        return **p;

      GPCL_THROW(bad_json_access());
    }

    decltype(auto) key() const { return pair().first; }

    decltype(auto) value() const { return pair().second; }

    decltype(auto) operator++()
    {
      visit([](auto &it) { ++it; }, data_);
      return *this;
    }

    decltype(auto) operator++(int)
    {
      auto result = *this;
      ++*this;
      return result;
    }

    decltype(auto) operator--()
    {
      visit([](auto &it) { --it; }, data_);
      return *this;
    }

    decltype(auto) operator--(int)
    {
      auto result = *this;
      --*this;
      return result;
    }

    bool operator==(const_iterator const &rhs) const
    {
      return data_ == rhs.data_;
    }

    bool operator!=(const_iterator const &rhs) const
    {
      return data_ != rhs.data_;
    }

    // bool operator<(iterator const &rhs) const { return data_ < rhs.data_; }
  };

  struct value_type
  {
    using data_type = basic_json::data_type;
    using string_type = basic_json::string_type;
    using object_type = basic_json::object_type;
    using array_type = basic_json::array_type;

    data_type data_;

    /// Construct a null json value.
    constexpr value_type() = default;

    constexpr value_type(value_type &&) noexcept = default;

    value_type(const value_type &other)
    {
      visit(clone_visitor{data_}, other.data_);
    }

    /// Construct a null json value.
    constexpr explicit value_type(null_tag) {}

    template <typename Type, typename... Args>
    constexpr explicit value_type(in_place_type_t<Type> tag, Args &&...args)
        : data_(tag, std::forward<Args>(args)...)
    {
    }

    template <typename... Args>
    constexpr explicit value_type(array_tag tag,
                                  std::initializer_list<value_type> il,
                                  Args &&...args)
        : data_(tag, il, std::forward<Args>(args)...)
    {
    }

    template <typename... Args>
    constexpr explicit value_type(
        object_tag tag,
        std::initializer_list<std::pair<const string_type, value_type>> il,
        Args &&...args)
        : data_(tag, il, std::forward<Args>(args)...)
    {
    }

    constexpr value_type(monostate) {}

    constexpr value_type(std::nullptr_t) {}

    template <typename T,
              std::enable_if_t<
                  std::is_integral_v<T> && !std::is_same_v<T, bool>, int> = 0>
    constexpr value_type(T v)
        : value_type(integer_tag{}, narrow_cast<integer_type>(v))
    {
    }

    template <typename T,
              std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    constexpr value_type(T v) : value_type(float_tag{}, v)
    {
    }

    template <typename B, std::enable_if_t<std::is_same_v<B, bool>, int> = 0>
    constexpr value_type(B v) : value_type(boolean_tag{}, v)
    {
    }

    constexpr value_type(string_type s) : value_type(string_tag{}, std::move(s))
    {
    }

    constexpr value_type(object_type o) : value_type(object_tag{}, std::move(o))
    {
    }

    constexpr value_type(array_type a) : value_type(array_tag{}, std::move(a))
    {
    }

    value_type(std::initializer_list<value_type> il)
        : value_type(array_tag{}, il)
    {
    }

    value_type(
        std::initializer_list<std::pair<const string_type, value_type>> il)
        : value_type(object_tag{}, il)
    {
    }

    template <typename... Args>
    static value_type object(Args &&...args)
    {
      return value_type(object_tag{}, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static value_type
    object(std::initializer_list<std::pair<const string_type, value_type>> il,
           Args &&...args)
    {
      return value_type(object_tag{}, il, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static value_type array(Args &&...args)
    {
      return value_type(array_tag{}, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static value_type array(std::initializer_list<value_type> il,
                            Args &&...args)
    {
      return value_type(array_tag{}, il, std::forward<Args>(args)...);
    }

    template <std::size_t S>
    constexpr value_type(const CharType (&s)[S]) : value_type(string_tag{}, s)
    {
    }

    template <typename T, typename A,
              typename std::enable_if<std::is_convertible<T, value_type>::value,
                                      int>::type = 0>
    constexpr explicit value_type(VectorType<T, A> const &vec)
        : value_type(array_tag{}, vec.begin(), vec.end())
    {
    }

    template <typename K, typename V, typename A,
              typename std::enable_if<
                  std::is_convertible<
                      std::pair<const K, V>,
                      std::pair<const string_type, value_type>>::value,
                  int>::type = 0>
    constexpr explicit value_type(MapType<K, V, A> const &vec)
        : value_type(object_tag{}, vec.begin(), vec.end())
    {
    }

    value_type &operator=(value_type &&) noexcept = default;

    value_type &operator=(const value_type &other)
    {
      if (this == &other)
        return *this;
      visit(clone_visitor{data_}, other.data_);
      return *this;
    }

    value_type &operator=(monostate)
    {
      data_ = monostate{};
      return *this;
    }

    value_type &operator=(std::nullptr_t) { return *this = monostate{}; }

    value_type &operator=(integer_type n)
    {
      data_ = n;
      return *this;
    }

    template <typename B,
              std::enable_if_t<std::is_same_v<std::decay_t<B>, bool>, int> = 0>
    value_type &operator=(B b)
    {
      data_ = b;
      return *this;
    }

    template <typename T,
              std::enable_if_t<std::is_convertible_v<T, float_type> &&
                                   !std::is_convertible_v<T, integer_type>,
                               int> = 0>
    value_type &operator=(T v)
    {
      data_ = v;
      return *this;
    }

    value_type &operator=(string_type s)
    {
      data_ = std::move(s);
      return *this;
    }

    template <std::size_t S>
    value_type &operator=(const typename string_type::value_type (&a)[S])
    {
      data_.template emplace<string_type>(a);
      return *this;
    }

    value_type &operator=(array_type a)
    {
      data_ = std::move(a);
      return *this;
    }

    value_type &operator=(object_type o)
    {
      data_ = std::move(o);
      return *this;
    }

    template <typename T>
    T *value_ptr()
    {
      (void)holds_alternative<T>(data_); // to use static_assert

      return visit(
          make_cow_visitor(
              [](auto &v) -> T * {
                if constexpr (std::is_same_v<std::decay_t<decltype(v)>, T>)
                  return std::addressof(v);
                return nullptr;
              },
              data_),
          data_);
    }

    template <typename T>
    T const *value_ptr() const noexcept
    {
      (void)holds_alternative<T>(data_); // to use static_assert

      return visit(make_copy_visitor([](auto &v) -> T * {
                     if constexpr (std::is_same_v<std::decay_t<decltype(v)>, T>)
                       return std::addressof(v);
                     return nullptr;
                   }),
                   data_);
    }

    template <typename T>
    T const *const_value_ptr() const noexcept
    {
      return this->template value_ptr<T>();
    }

    template <typename T>
    T &value()
    {
      if (auto p = this->template value_ptr<T>())
        return *p;
      GPCL_THROW(bad_json_access());
    }

    template <typename T>
    const T &value() const
    {
      if (auto p = this->template value_ptr<T>())
        return *p;
      GPCL_THROW(bad_json_access());
    }

    template <typename T>
    const T &const_value() const
    {
      return this->template value<T>();
    }

    template <typename T>
    T to() const
    {
      return visit(make_const_visitor(
                       [](const auto &x) -> T { return json_cast<T>(x); }),
                   data_);
    }

    constexpr bool is_null() const noexcept
    {
      return holds_alternative<null_type>(data_);
    }

    constexpr bool is_boolean() const noexcept
    {
      return holds_alternative<boolean_type>(data_);
    }

    constexpr bool is_integer() const noexcept
    {
      return holds_alternative<integer_type>(data_);
    }

    constexpr bool is_float() const noexcept
    {
      return holds_alternative<float_type>(data_);
    }

    constexpr bool is_number() const noexcept
    {
      return is_integer() || is_float();
    }

    constexpr bool is_string() const noexcept
    {
      return holds_alternative<string_type>(data_) ||
             holds_alternative<shared_ptr<const string_type>>(data_);
    }

    constexpr bool is_object() const noexcept
    {
      return holds_alternative<object_type>(data_) ||
             holds_alternative<shared_ptr<const object_type>>(data_);
    }

    constexpr bool is_array() const noexcept
    {
      return holds_alternative<array_type>(data_) ||
             holds_alternative<shared_ptr<const array_type>>(data_);
    }

    iterator begin()
    {
      if (auto p = value_ptr<array_type>())
        return p->begin();

      if (auto p = value_ptr<object_type>())
        return p->begin();

      return iterator();
    }

    iterator end()
    {
      if (auto p = value_ptr<array_type>())
        return p->end();

      if (auto p = value_ptr<object_type>())
        return p->end();

      return iterator();
    }

    const_iterator begin() const
    {
      if (auto p = const_value_ptr<array_type>())
        return p->begin();

      if (auto p = const_value_ptr<object_type>())
        return p->begin();

      return const_iterator();
    }

    const_iterator end() const
    {
      if (auto p = const_value_ptr<array_type>())
        return p->end();

      if (auto p = const_value_ptr<object_type>())
        return p->end();

      return const_iterator();
    }

    const_iterator cbegin() const { return begin(); }

    const_iterator cend() const { return end(); }

    std::size_t size() const
    {
      return visit(make_const_visitor(json_size), data_);
    }

    void clear() noexcept
    {
      return visit(make_const_visitor([&](auto const &x) {
                     data_.template emplace<std::decay_t<decltype(x)>>();
                   }),
                   data_);
    }

    value_type &at(std::size_t index)
    {
      return visit(make_cow_visitor(json_at<value_type &>(index), data_),
                   data_);
    }

    value_type const &at(std::size_t index) const
    {
      return visit(make_const_visitor(json_at<value_type const &>(index)),
                   data_);
    }

    value_type &at(const string_type &s)
    {
      return visit(make_cow_visitor(json_at<value_type &>(s), data_), data_);
    }

    value_type const &at(const string_type &s) const
    {
      return visit(make_const_visitor(json_at<value_type const &>(s)), data_);
    }

    value_type &operator[](std::size_t index) { return at(index); }

    value_type const &operator[](std::size_t index) const { return at(index); }

    value_type &operator[](const string_type &s)
    {
      return visit(make_cow_visitor(json_index<value_type &>(s), data_), data_);
    }

    value_type const &operator[](const string_type &s) const
    {
      return visit(make_const_visitor(json_index<value_type const &>(s)),
                   data_);
    }

    value_type &front()
    {
      auto &v = value<array_type>();
      if (v.empty())
        GPCL_THROW(bad_json_access());
      return v.front();
    }

    value_type const &front() const
    {
      auto &v = value<array_type>();
      if (v.empty())
        GPCL_THROW(bad_json_access());
      return v.front();
    }

    value_type &back()
    {
      auto &v = value<array_type>();
      if (v.empty())
        GPCL_THROW(bad_json_access());
      return v.back();
    }

    value_type const &back() const
    {
      auto &v = value<array_type>();
      if (v.empty())
        GPCL_THROW(bad_json_access());
      return v.back();
    }

    void push_back(value_type const &v) { value<array_type>().push_back(v); }

    void push_back(value_type &&v)
    {
      value<array_type>().push_back(std::move(v));
    }

    void push_back(CharType ch) { value<string_type>().push_back(ch); }

    void pop_back()
    {
      if (auto p = value_ptr<array_type>())
        return p->pop_back();

      value<string_type>().push_back();
    }

    std::pair<iterator, bool>
    insert(std::pair<const string_type, value_type> const &v)
    {
      return value<object_type>().insert(v);
    }

    std::pair<iterator, bool>
    insert(std::pair<const string_type, value_type> &&v)
    {
      return value<object_type>().insert(std::move(v));
    }

    template <typename T>
    std::pair<iterator, bool> insert(const string_type &key, T &&mapped)
    {
      return insert(std::pair<const string_type, value_type>(
          key, std::forward<T>(mapped)));
    }

    void erase(std::size_t pos, std::size_t count = 1)
    {
      if (auto p = value_ptr<array_type>())
      {
        auto it = p->cbegin();
        auto first = it + pos;
        auto last = first + count;
        p->erase(first, last);
        return;
      }

      value<string_type>().erase(pos, count);
    }

    void erase(const_iterator it, std::size_t count)
    {
      value<array_type>().erase(
          get<typename array_type::const_iterator>(it.data_), count);
    }

    void erase(const string_type &s) { value<object_type>().erase(s); }

    void erase(const_iterator it)
    {
      if (auto p = value_ptr<array_type>())
        return p->erase(get<typename array_type::const_iterator>(it.data_));

      if (auto p = value_ptr<object_type>())
        return p->erase(get<typename object_type::const_iterator>(it.data_));

      GPCL_THROW(bad_json_access());
    }

    friend inline ostream_type &operator<<(ostream_type &os,
                                           value_type const &v)
    {
      typename ostream_type::sentry sentry(os);
      if (!sentry)
        return os;

      gpcl::visit(make_const_visitor(serializing_visitor{os}), v.data_);
      return os;
    }
  };

  struct parser
  {
    struct error_event
    {
    };

    struct null_event
    {
      static constexpr monostate value{};
    };
    struct true_event
    {
      static constexpr boolean_type value{true};
    };
    struct false_event
    {
      static constexpr boolean_type value{false};
    };
    struct integer_event
    {
      integer_type value;
    };
    struct float_event
    {
      float_type value;
    };
    struct string_event
    {
      string_type value;
    };
    struct start_array_event
    {
      std::size_t size_hint = 0;
    };
    struct end_array_event
    {
    };
    struct start_object_event
    {
      std::size_t size_hint = 0;
    };
    struct object_key_event
    {
      string_type value;
    };
    struct end_object_event
    {
    };

    using event_type =
        variant<error_event, null_event, true_event, false_event, integer_event,
                float_event, string_event, start_array_event, end_array_event,
                start_object_event, object_key_event, end_object_event>;

    struct parsing_literal
    {
      string_type chars;

      explicit parsing_literal(char ch) { chars.push_back(ch); }
    };

    struct parsing_number
    {
      string_type chars;

      explicit parsing_number(char ch) { chars.push_back(ch); }
    };

    struct parsing_string
    {
      string_type chars;

      bool quote_closed = false;
    };

    struct indeterminate_state
    {
    };

    using parsing_state = variant<indeterminate_state, parsing_literal,
                                  parsing_number, parsing_string>;

    parsing_state state_;

    template <typename Visitor>
    bool put(indeterminate_state, char ch, Visitor &&visitor)
    {
      if (std::isspace(ch))
        return true;

      if (std::isalpha(ch))
      {
        state_ = parsing_literal(ch);
        return true;
      }

      if (std::isdigit(ch) || ch == '+' || ch == '-')
      {
        state_ = parsing_number(ch);
        return true;
      }

      if (ch == '"')
      {
        state_ = parsing_string();
        return true;
      }

      if (ch == '[')
      {
        visitor(start_array_event{});
        return true;
      }

      if (ch == ',')
      {
        return true;
      }

      if (ch == ']')
      {
        visitor(end_array_event{});
        return true;
      }

      if (ch == '{')
      {
        visitor(start_object_event{});
        return true;
      }

      if (ch == '}')
      {
        visitor(end_object_event{});
        return true;
      }

      visitor(error_event{});
      return false;
    }

    template <typename Visitor>
    bool put(parsing_literal &s, char ch, Visitor &&visitor)
    {
      if (std::isalnum(ch))
      {
        s.chars.push_back(ch);
        return true;
      }

      if (s.chars == "null")
        visitor(null_event{});
      else if (s.chars == "true")
        visitor(true_event{});
      else if (s.chars == "false")
        visitor(false_event{});
      else
        visitor(error_event{});

      state_ = indeterminate_state{};
      return false;
    }

    template <typename Visitor>
    bool put(parsing_number &s, char ch, Visitor &&visitor)
    {
      if (std::isdigit(ch) || ch == '.')
      {
        s.chars.push_back(ch);
        return true;
      }

      if (s.chars.find('.') != string_type::npos)
        visitor(float_event{json_cast<float_type>(s.chars)});
      else
        visitor(integer_event{json_cast<integer_type>(s.chars)});

      state_ = indeterminate_state{};
      return false;
    }

    template <typename Visitor>
    bool put(parsing_string &s, char ch, Visitor &&visitor)
    {
      if (!s.quote_closed)
      {
        if (ch != '"')
        {
          s.chars.push_back(ch);
          return true;
        }
        else
        {
          s.quote_closed = true;
          return true;
        }
      }
      else
      {
        if (std::isspace(ch))
          return true;

        if (ch == ':')
        {
          visitor(object_key_event{s.chars});
          return true;
        }

        visitor(string_event{s.chars});
        state_ = indeterminate_state{};
        return false;
      }
    }

    template <typename InputIt, typename Visitor>
    void put(InputIt first, InputIt last, Visitor &&visitor)
    {
      while (first != last)
      {
        char ch = *first++;
        bool consumed;

        do
        {
          consumed = visit(
              [this, ch, &visitor](auto &state) -> bool {
                return put(state, ch, std::forward<Visitor>(visitor));
              },
              state_);

        } while (!consumed);
      }
    }
  };

  struct value_builder
  {
    struct toplevel_context
    {
      value_type value;
    };

    struct array_context
    {
      array_type array;
    };

    struct object_context
    {
      object_type object;
      string_type key;
    };

    using context_type = variant<toplevel_context, array_context, object_context>;

    std::vector<context_type> stack_ { };

    value_builder()
    {
      stack_.emplace_back( toplevel_context{} );
    }

    template <typename Event>
    void handle_event(Event &&e)
    {
      handle_value(std::forward<Event>(e).value);
    }

    void handle_event(typename parser::start_array_event e)
    {
      array_context ctx;
      ctx.array.reserve(e.size_hint);
      stack_.push_back ( std::move(ctx) );
    }

    void handle_event(typename parser::end_array_event)
    {
      auto array = get<array_context>(std::move(stack_.back())).array;
      stack_.pop_back();
      handle_value(std::move(array));
    }

    void handle_event(typename parser::start_object_event)
    {
      stack_.push_back( object_context{} );
    }

    void handle_event(typename parser::object_key_event key)
    {
      get<object_context>(stack_.back()).key = std::move(key).value;
    }

    void handle_event(typename parser::end_object_event)
    {
      auto object = get<object_context>(std::move(stack_.back())).object;
      stack_.pop_back();
      handle_value( std::move(object) );
    }

    void handle_value ( toplevel_context &ctx, value_type v )
    {
      ctx.value = std::move(v);
    }

    void handle_value ( array_context &ctx, value_type v )
    {
      ctx.array.push_back( std::move(v) );
    }

    void handle_value ( object_context &ctx, value_type v )
    {
      ctx.object[ctx.key] = std::move(v);
    }

    void handle_value (value_type v)
    {
      visit([v, this](auto &ctx) mutable {
        handle_value(ctx, std::move(v));
      }, stack_.back());
    }

    value_type get_value()
    {
      if (stack_.size() != 1)
      {
        GPCL_THROW(bad_json_access());
      }

      return get<toplevel_context>(stack_[0]).value;
    }

    void handle_event(typename parser::error_event)
    {
      GPCL_THROW(bad_json_access());
    }

    template <typename E>
    void operator()(E &&e)
    {
      handle_event(std::forward<E>(e));
    }
  };

  static value_type parse(const std::string &text)
  {
    parser p;
    value_builder b;

    p.put(text.begin(), text.end(), b);

    return b.get_value();
  }
};

template <typename K, typename V, typename A>
class my_map : public std::map<K, V, std::less<K>, A>
{
  using base_type = std::map<K, V, std::less<K>, A>;

public:
  using std::map<K, V, std::less<K>, A>::map;

  using std::map<K, V, std::less<K>, A>::operator=;

  my_map(base_type const &b) : base_type(b) {}

  my_map(base_type &&b) noexcept : base_type(std::move(b)) {}
};

using json =
    basic_json<std::int64_t, double, std::basic_string, my_map, std::vector>;

} // namespace gpcl::detail

#endif // GPCL_DETAIL_JSON_HPP
