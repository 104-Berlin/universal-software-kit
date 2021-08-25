#pragma once

#define EXPAND(x) x

#define E_REM(...) __VA_ARGS__
#define E_EAT(...)


#define E_STRINGIFY(x) E_STRINGIFY1(x)
#define E_STRINGIFY1(x) E_STRINGIFY2(x)
#define E_STRINGIFY2(x) E_STRINGIFY3(x)
#define E_STRINGIFY3(x) E_STRINGIFY4(x)
#define E_STRINGIFY4(x) #x

// Retrieve the type
#define E_TYPEOF(x) E_DETAIL_TYPEOF(E_DETAIL_TYPEOF_PROBE x,)
#define E_DETAIL_TYPEOF(...) E_DETAIL_TYPEOF_HEAD(__VA_ARGS__)
#define E_DETAIL_TYPEOF_HEAD(x, ...) E_REM x
#define E_DETAIL_TYPEOF_PROBE(...) (__VA_ARGS__),
// Strip off the type
#define E_STRIP(x) E_EAT x
// Show the type without parenthesis
#define E_PAIR(x) E_REM x

#define E_CONCATENATE(arg1, arg2) E_CONCATENATE1(arg1, arg2)
#define E_CONCATENATE1(arg1, arg2) E_CONCATENATE2(arg1, arg2)
#define E_CONCATENATE2(arg1, arg2) E_CONCATENATE3(arg1, arg2)
#define E_CONCATENATE3(arg1, arg2) arg1##arg2

#define E_ARG_COUNT(...) EXPAND(E_PP_SIZE(__VA_ARGS__, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,))
#define E_PP_SIZE(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15, e16, e17, e18, e19, e20, e21, e22, e23, e24, e25, e26, e27, e28, e29, e30, e31, e32, e33, e34, e35, e36, e37, e38, e39, e40, e41, e42, e43, e44, e45, e46, e47, e48, e49, e50, e51, e52, e53, e54, e55, e56, e57, e58, e59, e60, e61, e62, e63, size, ...) size







#define E_LOOP_ARGS_1(cb, entry, ...) cb(entry)
#define E_LOOP_ARGS_2(cb, entry, ...) cb(entry)\
                                        E_LOOP_ARGS_1(cb, __VA_ARGS__)
#define E_LOOP_ARGS_3(cb, entry, ...) cb(entry)\
                                        E_LOOP_ARGS_2(cb, __VA_ARGS__)
#define E_LOOP_ARGS_4(cb, entry, ...) cb(entry)\
                                        E_LOOP_ARGS_3(cb, __VA_ARGS__)
#define E_LOOP_ARGS_5(cb, entry, ...) cb(entry)\
                                        E_LOOP_ARGS_4(cb, __VA_ARGS__)
#define E_LOOP_ARGS_6(cb, entry, ...) cb(entry)\
                                        E_LOOP_ARGS_5(cb, __VA_ARGS__)
#define E_LOOP_ARGS_7(cb, entry, ...) cb(entry)\
                                        E_LOOP_ARGS_6(cb, __VA_ARGS__)
#define E_LOOP_ARGS_8(cb, entry, ...) cb(entry)\
                                        E_LOOP_ARGS_7(cb, __VA_ARGS__)
#define E_LOOP_ARGS_9(cb, entry, ...) cb(entry)\
                                        E_LOOP_ARGS_8(cb, __VA_ARGS__)
#define E_LOOP_ARGS_10(cb, entry, ...) cb(entry)\
                                        E_LOOP_ARGS_9(cb, __VA_ARGS__)




#define E_LOOP_ARGS(callback, ...) E_CONCATENATE(E_LOOP_ARGS, E_ARG_COUNT(__VA_ARGS__))(callback, __VA_ARGS__)