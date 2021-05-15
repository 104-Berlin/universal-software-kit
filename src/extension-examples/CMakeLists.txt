add_library(Example1 MODULE example1.cpp)
add_library(Example2 MODULE example2.cpp)

set_target_properties(Example1 PROPERTIES SUFFIX ".uex")
set_target_properties(Example1 PROPERTIES PREFIX "")

set_target_properties(Example2 PROPERTIES SUFFIX ".uex")
set_target_properties(Example2 PROPERTIES PREFIX "")