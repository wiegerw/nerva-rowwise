project(NERVA_TOOLS)

add_executable(mlp mlp.cpp)
target_link_libraries(mlp LINK_PUBLIC nervalib Eigen3::Eigen MKL::MKL Python3::Python pybind11::pybind11)
set_target_properties(mlp PROPERTIES INSTALL_RPATH "${MKL_ROOT}/lib")

add_executable(mkl mkl.cpp)
target_link_libraries(mkl LINK_PUBLIC nervalib Eigen3::Eigen MKL::MKL)
set_target_properties(mkl PROPERTIES INSTALL_RPATH "${MKL_ROOT}/lib")

install(TARGETS mlp mkl RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
