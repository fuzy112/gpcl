//
// json.hpp
// ~~~~~~~~
//
// Copyright (c) 2021-2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_JSON_HPP
#define GPCL_JSON_HPP

#include <gpcl/default_allocator.hpp>
#include <gpcl/error.hpp>
#include <gpcl/lexical_cast.hpp>
#include <gpcl/shared_ptr.hpp>
#include <gpcl/variant.hpp>

#include <gpcl/json_at.hpp>
#include <gpcl/json_cast.hpp>
#include <gpcl/json_error.hpp>
#include <gpcl/json_index.hpp>
#include <gpcl/json_iomanip.hpp>
#include <gpcl/json_size.hpp>
#include <gpcl/json_tag.hpp>

#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <type_traits>
#include <vector>

namespace gpcl {

/// JSON data types.
template <
    typename IntegerType, typename FloatType,
    template <typename CharType, typename CharTraits, typename Allocator>
    class StringType,
    template <typename Key, typename Mapped, typename Allocator> class MapType,
    template <typename Value, typename Allocator> class VectorType,

    typename CharType = char, typename CharTraits = std::char_traits<CharType>,
    typename Allocator = gpcl::default_allocator<char>>
struct basic_json
{
  using char_type = CharType;
  using char_traits = CharTraits;

  using integer_type = IntegerType;
  using real_type = FloatType;
  using boolean_type = bool;
  using null_type = monostate;

  class value;

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
      MapType<string_type, value,
              rebind_allocator_type<std::pair<const string_type, value>>>;
  using array_type = VectorType<value, rebind_allocator_type<value>>;

  using print_style = gpcl::json_print_style;

private:
  using data_type = variant<null_type, boolean_type, integer_type, real_type,
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

  struct dereference_ptr_impl
  {
    template <typename T>
    T &&operator()(T &&x) const
    {
      return std::forward<T>(x);
    }

    template <typename T>
    T &operator()(const shared_ptr<T> &ptr) const
    {
      return *ptr;
    }
  };

  constexpr static const dereference_ptr_impl dereference_ptr{};

  template <typename F>
  struct const_visitor
  {
    F f_;

    template <typename T>
    explicit const_visitor(T &&t) : f_(std::forward<T>(t))
    {
    }

    template <typename... Ts>
    decltype(auto) operator()(const Ts &...xs) const
    {
      return f_(dereference_ptr(xs)...);
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

public:
  using ostream_type = std::basic_ostream<CharType, CharTraits>;

  using istream_type = std::basic_istream<CharType, CharTraits>;

public:
  /// Shared state of serializing_visitor.
  struct serializer
  {
    ostream_type &os_;

    string_type indent_item_;
    int indent_;

    explicit serializer(ostream_type &os,
                        const string_type &indent_item = "    ")
        : os_(os),
          indent_item_(indent_item),
          indent_(0)
    {
    }
  };

  /// Used to serialize a JSON value.
  struct serializing_visitor
  {
  private:
    serializer &ser_;

    /// A sentry class used to generate indentation.
    class indent_sentry
    {
      const serializing_visitor &vis_;

    public:
      explicit indent_sentry(const serializing_visitor &vis) : vis_(vis)
      {
        if (get_json_print_style(vis_.get_ostream()) ==
            print_style::json_print_compressed)
          return;

        ++vis_.indent();
        vis_.newline();
      }

      ~indent_sentry()
      {
        if (get_json_print_style(vis_.get_ostream()) ==
            print_style::json_print_compressed)
          return;

        --vis_.indent();
        vis_.newline();
      }
    };

    int &indent() const { return ser_.indent_; }

    const string_type &indent_item() const { return ser_.indent_item_; }

    void newline() const
    {
      if (get_json_print_style(get_ostream()) ==
          print_style::json_print_compressed)
        return;

      get_ostream() << '\n';
      for (int i = 0; i != indent(); ++i)
      {
        get_ostream() << indent_item();
      }
    }

    void space() const
    {
      if (get_json_print_style(get_ostream()) ==
          print_style::json_print_compressed)
        return;

      get_ostream() << ' ';
    }

  public:
    explicit serializing_visitor(serializer &ser) : ser_(ser) {}

    ostream_type &get_ostream() const { return ser_.os_; }

    void operator()(null_type) const { get_ostream() << "null"; }

    void operator()(integer_type i) const { get_ostream() << i; }

    void operator()(real_type f) const { get_ostream() << f; }

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
      get_ostream() << '[';

      auto iter = a.cbegin();
      auto const end = a.cend();
      if (iter != end)
      {
        indent_sentry ind(*this);

        visit(*this, iter->data_);

        while (++iter != end && get_ostream())
        {
          get_ostream() << ',';
          newline();
          visit(*this, iter->data_);
        }
      }
      get_ostream() << ']';
    }

    void operator()(const object_type &o) const
    {
      get_ostream() << '{';

      auto iter = o.cbegin();
      auto const end = o.cend();
      if (iter != end)
      {
        indent_sentry ind2(*this);

        get_ostream() << std::quoted(iter->first) << ':';
        space();
        visit(*this, iter->second.data_);

        while (++iter != end && get_ostream())
        {
          get_ostream() << ',';
          newline();

          get_ostream() << std::quoted(iter->first) << ':';
          space();
          visit(*this, iter->second.data_);
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

public:
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

    constexpr iterator(const iterator &) = default;

    constexpr iterator &operator=(const iterator &) = default;

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

      throw_json_error(json_errc::operation_not_supported,
                       "iterator<[Array]>::operator*:", true);
    }

    decltype(auto) operator->() const { return std::addressof(**this); }

    decltype(auto) pair() const
    {
      if (auto *p = get_if<typename object_type::iterator>(&data_))
        return **p;

      throw_json_error(json_errc::operation_not_supported,
                       "iterator<[Object]>::pair", true);
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

    constexpr const_iterator(const const_iterator &) = default;

    constexpr const_iterator &operator=(const const_iterator &) = default;

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

      throw_json_error(json_errc::operation_not_supported,
                       "const_iterator<[Array]>::operator*:", true);
    }

    decltype(auto) operator->() const { return std::addressof(**this); }

    decltype(auto) pair() const
    {
      if (auto *p = get_if<typename object_type::const_iterator>(&data_))
        return **p;

      throw_json_error(json_errc::operation_not_supported,
                       "const_iterator<[Object]>::pair:", true);
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

private:
  struct access;

public:
  struct parser;

public:
  /// JSON value type.
  class value
  {
  public:
    using string_type = basic_json::string_type;
    using object_type = basic_json::object_type;
    using array_type = basic_json::array_type;
    using boolean_type = basic_json::boolean_type;
    using integer_type = basic_json::integer_type;
    using real_type = basic_json::real_type;
    using null_type = basic_json::null_type;

    using string_tag = json_string_tag;
    using object_tag = json_object_tag;
    using array_tag = json_array_tag;
    using boolean_tag = json_boolean_tag;
    using integer_tag = json_integer_tag;
    using real_tag = json_real_tag;
    using null_tag = json_null_tag;

  private:
    friend struct basic_json::access;
    friend struct basic_json::serializer;
    friend struct basic_json::serializing_visitor;
    friend struct basic_json::iterator;
    friend struct basic_json::const_iterator;

  private:
    data_type data_;

  public:
    /// @name Constructors
    /// @{

    /// Construct a null json value.
    constexpr value() = default;

    /// Move constructor.
    constexpr value(value &&) noexcept = default;

    /// Copy constructor.
    value(const value &other) { visit(clone_visitor{data_}, other.data_); }

    /// Construct a null json value.
    constexpr explicit value(null_tag) {}

    /// Tagged constructor.
    // template <typename Type, typename... Args>
    // constexpr explicit value(in_place_type_t<Type> tag, Args &&...args)
    //     : data_(tag, std::forward<Args>(args)...)
    // {
    // }

    template <typename... Args>
    constexpr explicit value(integer_tag, Args &&...args)
        : data_(in_place_type<integer_type>, static_cast<Args &&>(args)...)
    {
    }

    template <typename... Args>
    constexpr explicit value(real_tag, Args &&...args)
        : data_(in_place_type<real_type>, static_cast<Args &&>(args)...)
    {
    }

    template <typename... Args>
    constexpr explicit value(boolean_tag, Args &&...args)
        : data_(in_place_type<boolean_type>, static_cast<Args &&>(args)...)
    {
    }

    /// Constructs an array.
    template <typename... Args>
    constexpr explicit value(array_tag, std::initializer_list<value> il,
                             Args &&...args)
        : data_(in_place_type<array_type>, il, std::forward<Args>(args)...)
    {
    }

    template <typename... Args>
    constexpr explicit value(array_tag, Args &&...args)
        : data_(in_place_type<array_type>, std::forward<Args>(args)...)
    {
    }

    /// Construct an object.
    template <typename... Args>
    constexpr explicit value(
        object_tag,
        std::initializer_list<std::pair<const string_type, value>> il,
        Args &&...args)
        : data_(in_place_type<object_type>, il, std::forward<Args>(args)...)
    {
    }

    template <typename... Args>
    constexpr explicit value(object_tag, Args &&...args)
        : data_(in_place_type<object_type>, static_cast<Args &&>(args)...)
    {
    }

    template <typename... Args>
    constexpr explicit value(null_tag, Args &&...args)
        : data_(in_place_type<null_type>, static_cast<Args &&>(args)...)
    {
    }

    /// Construct a null.
    constexpr value(monostate) {}

    /// Construct a null.
    constexpr value(std::nullptr_t) {}

    /// Construct an integer.
    template <typename T,
              std::enable_if_t<
                  std::is_integral_v<T> && !std::is_same_v<T, bool>, int> = 0>
    constexpr value(T v) : value(integer_tag{}, narrow_cast<integer_type>(v))
    {
    }

    /// Construct a real pointer number.
    template <typename T,
              std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    constexpr value(T v) : value(real_tag{}, v)
    {
    }

    /// Construct a boolean value.
    template <typename B, std::enable_if_t<std::is_same_v<B, bool>, int> = 0>
    constexpr value(B v) : value(boolean_tag{}, v)
    {
    }

    template <typename... Args>
    explicit value(string_tag, Args &&...args)
        : data_(in_place_type<string_type>, static_cast<Args &&>(args)...)
    {
    }

    /// Construct a string.
    constexpr value(string_type s) : value(string_tag{}, std::move(s)) {}

    /// Construct an object.
    constexpr value(object_type o) : value(object_tag{}, std::move(o)) {}

    /// Construct an array.
    constexpr value(array_type a) : value(array_tag{}, std::move(a)) {}

    /// Construct an array.
    value(std::initializer_list<value> il) : value(array_tag{}, il) {}

    /// Construct an object.
    value(std::initializer_list<std::pair<const string_type, value>> il)
        : value(object_tag{}, il)
    {
    }

    static value null(null_type = null_type()) { return value(); }

    static value boolean(boolean_type b) { return value(boolean_tag{}, b); }

    static value integer(integer_type i) { return value(integer_tag{}, i); }

    static value real(real_type f) { return value(real_tag{}, f); }

    template <typename... Args>
    static value string(Args &&...args)
    {
      return value(string_tag{}, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static value string(std::initializer_list<CharType> il, Args &&...args)
    {
      return value(string_tag{}, il, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static value object(Args &&...args)
    {
      return value(object_tag{}, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static value
    object(std::initializer_list<std::pair<const string_type, value>> il,
           Args &&...args)
    {
      return value(object_tag{}, il, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static value array(Args &&...args)
    {
      return value(array_tag{}, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static value array(std::initializer_list<value> il, Args &&...args)
    {
      return value(array_tag{}, il, std::forward<Args>(args)...);
    }

    template <std::size_t S>
    constexpr value(const CharType (&s)[S]) : value(string_tag{}, s)
    {
    }

    template <typename T, typename A,
              typename std::enable_if<std::is_convertible<T, value>::value,
                                      int>::type = 0>
    constexpr explicit value(VectorType<T, A> const &vec)
        : value(array_tag{}, vec.begin(), vec.end())
    {
    }

    template <
        typename K, typename V, typename A,
        typename std::enable_if<
            std::is_convertible<std::pair<const K, V>,
                                std::pair<const string_type, value>>::value,
            int>::type = 0>
    constexpr explicit value(MapType<K, V, A> const &vec)
        : value(object_tag{}, vec.begin(), vec.end())
    {
    }

    /// @}

    /// @name Assignment operators
    /// @{

    value &operator=(value &&other) noexcept
    {
      data_ = std::move(other.data_);
      return *this;
    }

    value &operator=(const value &other)
    {
      if (this == &other)
        return *this;
      visit(clone_visitor{data_}, other.data_);
      return *this;
    }

    value &operator=(monostate)
    {
      data_ = monostate{};
      return *this;
    }

    value &operator=(std::nullptr_t) { return *this = monostate{}; }

    value &operator=(integer_type n)
    {
      data_ = n;
      return *this;
    }

    template <typename B,
              std::enable_if_t<std::is_same_v<std::decay_t<B>, bool>, int> = 0>
    value &operator=(B b)
    {
      data_ = b;
      return *this;
    }

    template <typename T,
              std::enable_if_t<std::is_convertible_v<T, real_type> &&
                                   !std::is_convertible_v<T, integer_type>,
                               int> = 0>
    value &operator=(T v)
    {
      data_ = v;
      return *this;
    }

    value &operator=(string_type s)
    {
      data_ = std::move(s);
      return *this;
    }

    template <std::size_t S>
    value &operator=(const CharType (&a)[S])
    {
      data_.template emplace<string_type>(a);
      return *this;
    }

    value &operator=(array_type a)
    {
      data_ = std::move(a);
      return *this;
    }

    value &operator=(object_type o)
    {
      data_ = std::move(o);
      return *this;
    }

    /// @}

    /// @name Accessing JSON value.
    /// @{

#if defined _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4702)
#endif

    template <typename T>
    T *get_if()
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
    T const *get_if() const noexcept
    {
      (void)holds_alternative<T>(data_); // to use static_assert

      return visit(make_copy_visitor([](auto &v) -> T * {
                     if constexpr (std::is_same_v<std::decay_t<decltype(v)>, T>)
                       return std::addressof(v);
                     return nullptr;
                   }),
                   data_);
    }

#if defined _MSC_VER
#  pragma warning(pop)
#endif

    template <typename T>
    T const *const_get_if() const noexcept
    {
      return this->template get_if<T>();
    }

    template <typename T>
    T &get()
    {
      if (auto p = this->template get_if<T>())
        return *p;
      throw_json_error(json_errc::type_mismatch, "value::get<T>()", true);
    }

    template <typename T>
    const T &get() const
    {
      if (auto p = this->template get_if<T>())
        return *p;
      throw_json_error(json_errc::type_mismatch, "value::get<T>() const", true);
    }

    template <typename T>
    const T &const_get() const
    {
      return this->template get<T>();
    }

    template <typename T>
    T to() const
    {
      return visit(make_const_visitor(
                       [](const auto &x) -> T { return json_cast<T>(x); }),
                   data_);
    }

    /// @}

    /// @name Type checking functions
    /// @{

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
      return holds_alternative<real_type>(data_);
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

    /// @}

    /// @name Iterators
    /// @{

    iterator begin()
    {
      if (auto p = get_if<array_type>())
        return p->begin();

      if (auto p = get_if<object_type>())
        return p->begin();

      return iterator();
    }

    iterator end()
    {
      if (auto p = get_if<array_type>())
        return p->end();

      if (auto p = get_if<object_type>())
        return p->end();

      return iterator();
    }

    const_iterator begin() const
    {
      if (auto p = const_get_if<array_type>())
        return p->begin();

      if (auto p = const_get_if<object_type>())
        return p->begin();

      return const_iterator();
    }

    const_iterator end() const
    {
      if (auto p = const_get_if<array_type>())
        return p->end();

      if (auto p = const_get_if<object_type>())
        return p->end();

      return const_iterator();
    }

    const_iterator cbegin() const { return begin(); }

    const_iterator cend() const { return end(); }

    /// @}

    /// @name STL-like accesses
    /// @{

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

    value &at(std::size_t index)
    {
      return visit(make_cow_visitor(json_at<value &>(index), data_), data_);
    }

    value const &at(std::size_t index) const
    {
      return visit(make_const_visitor(json_at<value const &>(index)), data_);
    }

    value &at(const string_type &s)
    {
      return visit(make_cow_visitor(json_at<value &>(s), data_), data_);
    }

    value const &at(const string_type &s) const
    {
      return visit(make_const_visitor(json_at<value const &>(s)), data_);
    }

    value &operator[](std::size_t index) { return at(index); }

    value const &operator[](std::size_t index) const { return at(index); }

    value &operator[](const string_type &s)
    {
      return visit(make_cow_visitor(json_index<value &>(s), data_), data_);
    }

    value const &operator[](const string_type &s) const
    {
      return visit(make_const_visitor(json_index<value const &>(s)), data_);
    }

    value &front()
    {
      auto &v = get<array_type>();
      if (v.empty())
        throw_json_error(json_errc::out_of_range, "value::front()", true);
      return v.front();
    }

    value const &front() const
    {
      auto &v = get<array_type>();
      if (v.empty())
        throw_json_error(json_errc::out_of_range, "value::front() const", true);
      return v.front();
    }

    value &back()
    {
      auto &v = get<array_type>();
      if (v.empty())
        throw_json_error(json_errc::out_of_range, "value::back()", true);
      return v.back();
    }

    value const &back() const
    {
      auto &v = get<array_type>();
      if (v.empty())
        throw_json_error(json_errc::out_of_range, "value::back() const", true);
      return v.back();
    }

    void push_back(value const &v) { get<array_type>().push_back(v); }

    void push_back(value &&v) { get<array_type>().push_back(std::move(v)); }

    void push_back(CharType ch) { get<string_type>().push_back(ch); }

    void pop_back()
    {
      if (auto p = get_if<array_type>())
        return p->pop_back();

      get<string_type>().push_back();
    }

    std::pair<iterator, bool>
    insert(std::pair<const string_type, value> const &v)
    {
      return get<object_type>().insert(v);
    }

    std::pair<iterator, bool> insert(std::pair<const string_type, value> &&v)
    {
      return get<object_type>().insert(std::move(v));
    }

    template <typename T>
    std::pair<iterator, bool> insert(const string_type &key, T &&mapped)
    {
      return insert(
          std::pair<const string_type, value>(key, std::forward<T>(mapped)));
    }

    void erase(std::size_t pos, std::size_t count = 1)
    {
      if (auto p = get_if<array_type>())
      {
        auto it = p->cbegin();
        auto first = it + pos;
        auto last = first + count;
        p->erase(first, last);
        return;
      }

      get<string_type>().erase(pos, count);
    }

    /// Erases @c count elements from an array.
    /// @pre is_array().
    void erase(const_iterator it, std::size_t count)
    {
      GPCL_ASSERT(is_array());
      get<array_type>().erase(
          get<typename array_type::const_iterator>(it.data_), count);
    }

    /// Erases a property from an object.
    /// @pre is_object().
    void erase(const string_type &s)
    {
      GPCL_ASSERT(is_object());
      get<object_type>().erase(s);
    }

    /// Erases an element.
    /// @pre is_array() || is_object().
    void erase(const_iterator it)
    {
      GPCL_ASSERT(is_array() || is_object());
      if (auto p = get_if<array_type>())
        return p->erase(get<typename array_type::const_iterator>(it.data_));

      if (auto p = get_if<object_type>())
        return p->erase(get<typename object_type::const_iterator>(it.data_));

      throw_json_error(json_errc::type_mismatch,
                       "value<[Array|Object]>::erase(const_iterator)", true);
    }

    /// @}
  };

private:
  /// The access is used to access the private data member of @c value.
  /// Since @c access is a internal class of basic_json, it is accessible
  /// in other internal classes of basic_json's.
  struct access
  {
    template <typename T>
    static auto &&data(T &&x)
    {
      return std::forward<T>(x).data_;
    }
  };

public:
  /// @name Serializing
  /// @{

  /// Serializes a JSON value.
  friend inline ostream_type &operator<<(ostream_type &os, value const &v)
  {
    typename ostream_type::sentry sentry(os);
    if (!sentry)
      return os;

    serializer ser(os);
    gpcl::visit(make_const_visitor(serializing_visitor(ser)), access::data(v));
    return os;
  }

  /// @}

  /// @name Equality operators
  /// @relates gpcl::basic_json::value
  /// @{

// clang-format off
#if defined _MSC_VER
# pragma warning(push)
# pragma warning(disable : 4702)
#endif
  // clang-format on

  friend inline bool operator==(const value &x, const value &y)
  {
    return visit(make_const_visitor([](const auto &w, const auto &z) {
                   if constexpr (std::is_same_v<decltype(w), decltype(z)>)
                     return w == z;
                   return false;
                 }),
                 access::data(x), access::data(y));
  }

// clang-format off
#if defined _MSC_VER
# pragma warning(pop)
#endif
  // clang-format on

  friend inline bool operator!=(const value &x, const value &y)
  {
    return !(x == y);
  }

  /// @}

  // friend inline bool operator>(const value &x, const value &y)
  // {
  //   return x.data_ > y.data_;
  // }

  // friend inline bool operator<=(const value &x, const value &y)
  // {
  //   return x.data_ <= x.data_;
  // }

  // friend inline bool operator>=(const value &x, const value &y)
  // {
  //   return x.data_ >= x.data_;
  // }

  /// JSON parser.
  struct parser
  {
    /// @name Events
    /// @{

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
      real_type value;
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

    /// @}

  private:
    /// @name Parser states
    /// @{

    struct parsing_literal
    {
      string_type chars;

      explicit parsing_literal(CharType ch) { chars.push_back(ch); }
    };

    struct parsing_number
    {
      string_type chars;

      explicit parsing_number(CharType ch) { chars.push_back(ch); }
    };

    struct parsing_string
    {
      string_type chars;

      bool quote_closed = false;
    };

    struct indeterminate_state
    {
    };

    /// @}

    using parsing_state = variant<indeterminate_state, parsing_literal,
                                  parsing_number, parsing_string>;

    parsing_state state_;

    template <typename Visitor>
    bool put(indeterminate_state, CharType ch, Visitor &&visitor)
    {
      if (std::isspace(ch))
        return true;

      if (std::isalpha(ch))
      {
        state_ = parsing_literal(ch);
        return true;
      }

      if (std::isdigit(ch) || CharTraits::eq(ch, '+') ||
          CharTraits::eq(ch, '+'))
      {
        state_ = parsing_number(ch);
        return true;
      }

      if (CharTraits::eq(ch, '"'))
      {
        state_ = parsing_string();
        return true;
      }

      if (CharTraits::eq(ch, '['))
      {
        visitor(start_array_event{});
        return true;
      }

      if (CharTraits::eq(ch, ','))
      {
        return true;
      }

      if (CharTraits::eq(ch, ']'))
      {
        visitor(end_array_event{});
        return true;
      }

      if (CharTraits::eq(ch, '{'))
      {
        visitor(start_object_event{});
        return true;
      }

      if (CharTraits::eq(ch, '}'))
      {
        visitor(end_object_event{});
        return true;
      }

      visitor(error_event{});
      return false;
    }

    template <typename Visitor>
    bool put(parsing_literal &s, CharType ch, Visitor &&visitor)
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
    bool put(parsing_number &s, CharType ch, Visitor &&visitor)
    {
      if (std::isdigit(ch) || CharTraits::eq(ch, '.'))
      {
        s.chars.push_back(ch);
        return true;
      }

      if (s.chars.find('.') != string_type::npos)
        visitor(float_event{json_cast<real_type>(s.chars)});
      else
        visitor(integer_event{json_cast<integer_type>(s.chars)});

      state_ = indeterminate_state{};
      return false;
    }

    template <typename Visitor>
    bool put(parsing_string &s, CharType ch, Visitor &&visitor)
    {
      if (!s.quote_closed)
      {
        if (!CharTraits::eq(ch, '"'))
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

        if (CharTraits::eq(ch, ':'))
        {
          visitor(object_key_event{std::move(s).chars});
          state_ = indeterminate_state{};
          return true;
        }

        visitor(string_event{std::move(s).chars});
        state_ = indeterminate_state{};
        return false;
      }
    }

    /// Checks if T meets the requirement of Visitor.
    template <typename T>
    using is_visitor = meta::apply<
        meta::quote<meta::and_>,
        meta::transform<
            meta::list<error_event, null_event, true_event, false_event,
                       integer_event, float_event, integer_event, float_event,
                       string_event, start_array_event, end_array_event,
                       start_object_event, object_key_event, end_object_event>,
            meta::lambda<class arg_event,
                         meta::defer<std::is_invocable, T &&, arg_event>>>>;

  public:
    /// Consume a sequence of characters and emit events to the visitor.
    /// @param first begin of the character sequence.
    /// @param last end of the character sequence.
    /// @param visitor an event visitor.
    ///
    /// @tparam Visitor a type that is callable with all the event types.
    ///
    /// @sa value_builder is a typical Visitor.
    template <typename InputIt, typename Visitor>
    void put(InputIt first, InputIt last, Visitor &&visitor)
    {
      static_assert(is_visitor<Visitor>::type::value);

      while (first != last)
      {
        CharType ch = *first++;
        bool consumed;

        do
        {
          consumed = visit(
              [this, ch, &visitor](auto &state) -> bool {
                return this->put(state, ch, std::forward<Visitor>(visitor));
              },
              state_);

        } while (!consumed);
      }
    }
  };

  /// JSON value builder.
  /// This class can be used as an EventVisitor of @c parser.
  template <typename Allocator1 = gpcl::default_allocator<char>>
  struct value_builder
  {

  private:
    /// Context states.
    /// @{

    struct toplevel_context
    {
      value value_;
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

    /// @}

    using context_type =
        variant<toplevel_context, array_context, object_context>;

    using stack_type =
        std::vector<context_type, typename std::allocator_traits<Allocator1>::
                                      template rebind_alloc<context_type>>;

    stack_type stack_;

  public:
    /// Construct a value_builder
    explicit value_builder(Allocator1 const &alloc = Allocator1())
        : stack_(alloc)
    {
      stack_.emplace_back(toplevel_context{});
    }

  private:
    template <typename Event>
    void handle_event(Event &&e)
    {
      handle_value(std::forward<Event>(e).value);
    }

    void handle_event(typename parser::start_array_event e)
    {
      array_context ctx;
      ctx.array.reserve(e.size_hint);
      stack_.push_back(std::move(ctx));
    }

    void handle_event(typename parser::end_array_event)
    {
      auto array = get<array_context>(std::move(stack_.back())).array;
      stack_.pop_back();
      handle_value(std::move(array));
    }

    void handle_event(typename parser::start_object_event)
    {
      stack_.push_back(object_context{});
    }

    void handle_event(typename parser::object_key_event key)
    {
      get<object_context>(stack_.back()).key = std::move(key).value;
    }

    void handle_event(typename parser::end_object_event)
    {
      auto object = get<object_context>(std::move(stack_.back())).object;
      stack_.pop_back();
      handle_value(std::move(object));
    }

    void handle_event(typename parser::error_event)
    {
      throw_json_error(json_errc::failed_to_parse, "parser", true);
    }

    void handle_value(toplevel_context &ctx, value v)
    {
      ctx.value_ = std::move(v);
    }

    void handle_value(array_context &ctx, value v)
    {
      ctx.array.emplace_back(std::move(v));
    }

    void handle_value(object_context &ctx, value v)
    {
      GPCL_ASSERT(!ctx.key.empty());
      ctx.object.insert(
          typename object_type::value_type(std::move(ctx.key), std::move(v)));
    }

    void handle_value(value v)
    {
      visit([v, this](
                auto &ctx) mutable { this->handle_value(ctx, std::move(v)); },
            stack_.back());
    }

  public:
    /// Retrieve the result value.
    value get_value()
    {
      if (stack_.size() != 1)
      {
        throw_json_error(json_errc::failed_to_parse,
                         "value_builder::get_value()", true);
      }

      return get<toplevel_context>(stack_[0]).value_;
    }

    /// Consume an event.
    /// @tparam E an event type.
    template <typename E>
    auto operator()(E &&e)
        -> std::void_t<decltype(this->handle_event(std::declval<E>()))>
    {
      handle_event(std::forward<E>(e));
    }
  };

  /// @name Parsing
  /// @{

  /// Parses a JSON text using @c parser and @c value_builder.
  template <typename InputIt,
            typename Allocator1 = gpcl::default_allocator<char>>
  static value parse(InputIt first, InputIt last,
                     Allocator1 const &alloc = Allocator1())
  {
    parser p;
    value_builder<Allocator1> builder(alloc);

    p.put(first, last, builder);

    return builder.get_value();
  }

  /// Parses a JSON text using @c parser and @c value_builder.
  template <typename Allocator1 = gpcl::default_allocator<char>>
  static value parse(std::basic_string_view<CharType, CharTraits> string,
                     Allocator1 const &alloc = Allocator1())
  {
    return parse(string.begin(), string.end(), alloc);
  }

  /// Parses a JSON text using @c parser and @c value_builder.
  template <typename Allocator1 = gpcl::default_allocator<char>>
  static value parse(std::basic_istream<CharType, CharTraits> &stream,
                     Allocator1 const &alloc = Allocator1())
  {
    return parse(std::istreambuf_iterator<CharType, CharTraits>(stream),
                 std::istreambuf_iterator<CharType, CharTraits>(), alloc);
  }

  /// @}
};

template <typename K, typename V, typename A>
using json_std_map_adaptor = std::map<K, V, std::less<K>, A>;

using json = basic_json<std::int64_t, double, std::basic_string,
                        json_std_map_adaptor, std::vector>;

} // namespace gpcl

#endif // GPCL_JSON_HPP
