/// @defgroup customization_point Customization Points
///
/// A customization point object is a function object
/// ([function.objects]) with a literal class type that interacts with
/// program-defined types while enforcing semantic requirements on that
/// interaction.
///
/// The type of a customization point object, ignoring cv-qualifiers, shall
/// model semiregular ([concepts.object]).
///
/// All instances of a specific customization point object type shall be equal
/// ([concepts.equality]). The effects of invoking different instances of a
/// specific customization point object type on the same arguments are
/// equivalent.
///
/// The type T of a customization point object, ignoring cv-qualifiers, shall
/// model invocable<T&, Args...>, invocable<const T&, Args...>, invocable<T,
/// Args...>, and invocable<const T, Args...> ([concept.invocable]) when the
/// types in Args... meet the requirements specified in that customization point
/// object's definition. When the types of Args... do not meet the customization
/// point object's requirements, T shall not have a function call operator that
/// participates in overload resolution.
///
/// For a given customization point object o, let p be a variable initialized as
/// if by auto p = o;. Then for any sequence of arguments args..., the following
/// expressions have effects equivalent to o(args...):
///   - `p(args...)`
///   - `as_const(p)(args...)`
///   - `std::move(p)(args...)`
///   - `std::move(as_const(p))(args...)`
///
/// Each customization point object type constrains its return type to model a
/// particular concept.
///
/// [Note 1: Many of the customization point objects in the library evaluate
/// function call expressions with an unqualified name which results in a call
/// to a program-defined function found by argument dependent name lookup
/// ([basic.lookup.argdep]). To preclude such an expression resulting in a call
/// to unconstrained functions with the same name in namespace std,
/// customization point objects specify that lookup for these expressions is
/// performed in a context that includes deleted overloads matching the
/// signatures of overloads defined in namespace std. When the deleted overloads
/// are viable, program-defined overloads need to be more specialized
/// ([temp.func.order]) or more constrained ([temp.constr.order]) to be used by
/// a customization point object. — end note]
///