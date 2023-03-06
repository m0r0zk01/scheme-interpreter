set(
    tests

    test_boolean
    test_control_flow
    test_eval
    test_fuzzing_1
    test_fuzzing_2
    test_integer
    test_lambda
    test_list
    test_pair_mut
    test_parser
    test_symbol
    test_tokenizer
)

foreach(test ${tests})
    add_executable(${test} ${test}.cpp)
    target_link_libraries(${test} catch allocations_checker)
    add_test(${test} ${test})
endforeach()
