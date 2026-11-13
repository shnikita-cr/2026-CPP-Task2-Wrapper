if(NOT DEFINED WRAPPER_APP)
    message(FATAL_ERROR "WRAPPER_APP must point to the demonstration executable")
endif()

execute_process(
    COMMAND "${WRAPPER_APP}"
    RESULT_VARIABLE demo_status
    OUTPUT_VARIABLE demo_output
    ERROR_VARIABLE demo_error
    TIMEOUT 10
    ENCODING UTF-8
)
if(NOT "${demo_status}" STREQUAL "0")
    message(FATAL_ERROR "Demonstration failed (${demo_status}): ${demo_error}")
endif()
if(NOT "${demo_error}" STREQUAL "")
    message(FATAL_ERROR "Demonstration wrote to stderr: ${demo_error}")
endif()
string(REPLACE "\r\n" "\n" demo_output "${demo_output}")

set(expected_steps
    "Initial document:\ncat sleeps. dog runs! ёж идёт? cat watches."
    "replace (defaults):\nfox sleeps. dog runs! ёж идёт? fox watches."
    "replace (override from=dog):\nfox sleeps. fox runs! ёж идёт? fox watches."
    "find (defaults):\nfound"
    "find (override phrase=missing phrase):\nnot found"
    "capitalize_sentences:\nFox sleeps. Fox runs! Ёж идёт? Fox watches."
    "capitalize_words:\nFox Sleeps. Fox Runs! Ёж Идёт? Fox Watches."
    "number_sentences:\n1. Fox Sleeps.\n2. Fox Runs!\n3. Ёж Идёт?\n4. Fox Watches."
    "order (defaults):\nOrder: 3 books (gift)"
    "order (override quantity=4, gift=false):\nOrder: 4 books (regular)"
)
set(previous_position -1)
foreach(expected IN LISTS expected_steps)
    string(FIND "${demo_output}" "${expected}" position)
    if(position EQUAL -1)
        message(FATAL_ERROR "Missing demonstration result:\n${expected}\nOutput:\n${demo_output}")
    endif()
    if(position LESS_EQUAL previous_position)
        message(FATAL_ERROR "Demonstration steps are out of order: ${expected}")
    endif()
    set(previous_position "${position}")
endforeach()
