cmake_minimum_required(VERSION 3.0)
project(test C)

enable_testing()

list(APPEND tests
    test_slist
    test_dlist
    test_rbtree
    test_ringbuf
    test_tree_rb
    test_tree_splay
)

foreach(test ${tests})
    add_executable(${test} ${test}.c)
    target_include_directories(${test} PRIVATE ..)
    add_test(NAME ${test} COMMAND "./${test}")
endforeach()
