add_library(Example1 MODULE example1.cpp)
set_target_properties(Example1 PROPERTIES SUFFIX ".uex")
set_target_properties(Example1 PROPERTIES PREFIX "")