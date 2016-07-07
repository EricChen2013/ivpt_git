# generated from genmsg/cmake/pkg-genmsg.cmake.em

message(STATUS "ivsensorgps: 1 messages, 0 services")

set(MSG_I_FLAGS "-Iivsensorgps:/home/raul/ivpt-git/sensor/src/ivsensorgps/msg;-Istd_msgs:/opt/ros/jade/share/std_msgs/cmake/../msg")

# Find all generators
find_package(gencpp REQUIRED)
find_package(geneus REQUIRED)
find_package(genlisp REQUIRED)
find_package(genpy REQUIRED)

add_custom_target(ivsensorgps_generate_messages ALL)

# verify that message/service dependencies have not changed since configure



get_filename_component(_filename "/home/raul/ivpt-git/sensor/src/ivsensorgps/msg/gpsmsg.msg" NAME_WE)
add_custom_target(_ivsensorgps_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "ivsensorgps" "/home/raul/ivpt-git/sensor/src/ivsensorgps/msg/gpsmsg.msg" ""
)

#
#  langs = gencpp;geneus;genlisp;genpy
#

### Section generating for lang: gencpp
### Generating Messages
_generate_msg_cpp(ivsensorgps
  "/home/raul/ivpt-git/sensor/src/ivsensorgps/msg/gpsmsg.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/ivsensorgps
)

### Generating Services

### Generating Module File
_generate_module_cpp(ivsensorgps
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/ivsensorgps
  "${ALL_GEN_OUTPUT_FILES_cpp}"
)

add_custom_target(ivsensorgps_generate_messages_cpp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_cpp}
)
add_dependencies(ivsensorgps_generate_messages ivsensorgps_generate_messages_cpp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/raul/ivpt-git/sensor/src/ivsensorgps/msg/gpsmsg.msg" NAME_WE)
add_dependencies(ivsensorgps_generate_messages_cpp _ivsensorgps_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(ivsensorgps_gencpp)
add_dependencies(ivsensorgps_gencpp ivsensorgps_generate_messages_cpp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS ivsensorgps_generate_messages_cpp)

### Section generating for lang: geneus
### Generating Messages
_generate_msg_eus(ivsensorgps
  "/home/raul/ivpt-git/sensor/src/ivsensorgps/msg/gpsmsg.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/ivsensorgps
)

### Generating Services

### Generating Module File
_generate_module_eus(ivsensorgps
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/ivsensorgps
  "${ALL_GEN_OUTPUT_FILES_eus}"
)

add_custom_target(ivsensorgps_generate_messages_eus
  DEPENDS ${ALL_GEN_OUTPUT_FILES_eus}
)
add_dependencies(ivsensorgps_generate_messages ivsensorgps_generate_messages_eus)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/raul/ivpt-git/sensor/src/ivsensorgps/msg/gpsmsg.msg" NAME_WE)
add_dependencies(ivsensorgps_generate_messages_eus _ivsensorgps_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(ivsensorgps_geneus)
add_dependencies(ivsensorgps_geneus ivsensorgps_generate_messages_eus)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS ivsensorgps_generate_messages_eus)

### Section generating for lang: genlisp
### Generating Messages
_generate_msg_lisp(ivsensorgps
  "/home/raul/ivpt-git/sensor/src/ivsensorgps/msg/gpsmsg.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/ivsensorgps
)

### Generating Services

### Generating Module File
_generate_module_lisp(ivsensorgps
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/ivsensorgps
  "${ALL_GEN_OUTPUT_FILES_lisp}"
)

add_custom_target(ivsensorgps_generate_messages_lisp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_lisp}
)
add_dependencies(ivsensorgps_generate_messages ivsensorgps_generate_messages_lisp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/raul/ivpt-git/sensor/src/ivsensorgps/msg/gpsmsg.msg" NAME_WE)
add_dependencies(ivsensorgps_generate_messages_lisp _ivsensorgps_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(ivsensorgps_genlisp)
add_dependencies(ivsensorgps_genlisp ivsensorgps_generate_messages_lisp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS ivsensorgps_generate_messages_lisp)

### Section generating for lang: genpy
### Generating Messages
_generate_msg_py(ivsensorgps
  "/home/raul/ivpt-git/sensor/src/ivsensorgps/msg/gpsmsg.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/ivsensorgps
)

### Generating Services

### Generating Module File
_generate_module_py(ivsensorgps
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/ivsensorgps
  "${ALL_GEN_OUTPUT_FILES_py}"
)

add_custom_target(ivsensorgps_generate_messages_py
  DEPENDS ${ALL_GEN_OUTPUT_FILES_py}
)
add_dependencies(ivsensorgps_generate_messages ivsensorgps_generate_messages_py)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/raul/ivpt-git/sensor/src/ivsensorgps/msg/gpsmsg.msg" NAME_WE)
add_dependencies(ivsensorgps_generate_messages_py _ivsensorgps_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(ivsensorgps_genpy)
add_dependencies(ivsensorgps_genpy ivsensorgps_generate_messages_py)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS ivsensorgps_generate_messages_py)



if(gencpp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/ivsensorgps)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/ivsensorgps
    DESTINATION ${gencpp_INSTALL_DIR}
  )
endif()
add_dependencies(ivsensorgps_generate_messages_cpp std_msgs_generate_messages_cpp)

if(geneus_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/ivsensorgps)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/ivsensorgps
    DESTINATION ${geneus_INSTALL_DIR}
  )
endif()
add_dependencies(ivsensorgps_generate_messages_eus std_msgs_generate_messages_eus)

if(genlisp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/ivsensorgps)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/ivsensorgps
    DESTINATION ${genlisp_INSTALL_DIR}
  )
endif()
add_dependencies(ivsensorgps_generate_messages_lisp std_msgs_generate_messages_lisp)

if(genpy_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/ivsensorgps)
  install(CODE "execute_process(COMMAND \"/usr/bin/python\" -m compileall \"${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/ivsensorgps\")")
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/ivsensorgps
    DESTINATION ${genpy_INSTALL_DIR}
  )
endif()
add_dependencies(ivsensorgps_generate_messages_py std_msgs_generate_messages_py)
