
set(MODULES_DIR ${CMAKE_CURRENT_LIST_DIR})

function (copy_module MODULE)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/../modules/${MODULE}

        COMMAND
            cp ${MODULES_DIR}/${MODULE} ${CMAKE_CURRENT_BINARY_DIR}/../modules/${MODULE}

        DEPENDS ${MODULES_DIR}/${MODULE}
    )

    target_sources(usermod_modules_py INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/../modules/${MODULE})
endfunction()

function (genversion VERSION_FILE)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/../modules/${VERSION_FILE}

        COMMAND
            bash ${MODULES_DIR}/genversion.sh ${CMAKE_CURRENT_BINARY_DIR}/../modules/${VERSION_FILE}

        DEPENDS ${MODULES_DIR}/genversion.sh
    )

    target_sources(usermod_modules_py INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/../modules/${VERSION_FILE})
endfunction()

# Create a dummy usermod to hang our .py copies from
add_library(usermod_modules_py INTERFACE)
target_link_libraries(usermod INTERFACE usermod_modules_py)

genversion(version.py)

# .py files to copy from modules_py to ports/rp2/modules
copy_module(pimoroni.py)