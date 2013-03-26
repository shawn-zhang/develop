FILE(REMOVE_RECURSE
  "CMakeFiles/demo.dir/demo.cpp.o"
  "libdemo.pdb"
  "libdemo.a"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang CXX)
  INCLUDE(CMakeFiles/demo.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
