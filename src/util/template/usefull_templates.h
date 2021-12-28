#pragma once

// Used to create has_member struct to be able to detect template to contain member m
#define HAS_MEMBER(m) \
	template <typename T, typename = void>\
	struct EXPAND ( E_CONCATENATE ( has_member_, m ) ) : std::false_type{};\
	\
	template <typename T>\
	struct EXPAND ( E_CONCATENATE ( has_member_, m ) )<T, decltype((void)T::m, void())> : std::true_type {};


template<typename T> struct is_vector : public std::false_type {};

template<typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type {};

template <typename T, typename ...Of>
constexpr bool is_one_of()
{
    if constexpr ((std::is_same<T, Of>() || ...))
    {
        return true;
    }
    return false;
}

template <typename T>
constexpr bool is_primitive = is_one_of<T, int, unsigned int, u32, float, double, EString, bool, u64, unsigned long long>() || is_vector<T>::value;
