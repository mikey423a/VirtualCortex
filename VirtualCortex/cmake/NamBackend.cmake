# Optional NAM backend for live grid playback and offline rendering. No configure-time fetch.
set(VC_NAM_SOURCE_DIR "" CACHE PATH "Pinned NeuralAmpModelerCore checkout (v0.5.4)")
if(VC_NAM_SOURCE_DIR)
  if(NOT EXISTS "${VC_NAM_SOURCE_DIR}/Dependencies/eigen/Eigen/Core")
    message(FATAL_ERROR "NAM requires its Eigen submodule; checkout recursively")
  endif()
  file(GLOB NAM_CORE_SOURCES CONFIGURE_DEPENDS
    "${VC_NAM_SOURCE_DIR}/NAM/*.cpp" "${VC_NAM_SOURCE_DIR}/NAM/*/*.cpp")
  # OBJECT preserves architecture registration units that an ordinary static
  # archive's linker could otherwise discard.
  add_library(vc_nam_core OBJECT ${NAM_CORE_SOURCES})
  target_compile_features(vc_nam_core PUBLIC cxx_std_20)
  target_compile_definitions(vc_nam_core PUBLIC NAM_SAMPLE_FLOAT NOMINMAX)
  target_include_directories(vc_nam_core SYSTEM PUBLIC
    "${VC_NAM_SOURCE_DIR}" "${VC_NAM_SOURCE_DIR}/Dependencies/eigen"
    "${VC_NAM_SOURCE_DIR}/Dependencies/nlohmann")
  add_library(vc_nam_live OBJECT Source/DSP/LiveNamAmp.cpp)
  target_include_directories(vc_nam_live PRIVATE Source)
  target_link_libraries(vc_nam_live PRIVATE vc_nam_core)
  set_target_properties(vc_nam_core vc_nam_live PROPERTIES POSITION_INDEPENDENT_CODE ON)
  if(BUILD_TESTING)
    add_executable(virtualcortex_live_nam_tests tests/test_live_nam.cpp $<TARGET_OBJECTS:vc_nam_live>)
    target_include_directories(virtualcortex_live_nam_tests PRIVATE Source)
    target_link_libraries(virtualcortex_live_nam_tests PRIVATE vc_nam_core)
    add_test(NAME live_nam_linear COMMAND virtualcortex_live_nam_tests)
    add_test(NAME live_nam_lstm COMMAND virtualcortex_live_nam_tests "${VC_NAM_SOURCE_DIR}/example_models/lstm.nam")
    add_test(NAME live_nam_wavenet COMMAND virtualcortex_live_nam_tests "${VC_NAM_SOURCE_DIR}/example_models/wavenet.nam")
    set_tests_properties(live_nam_linear live_nam_lstm live_nam_wavenet PROPERTIES TIMEOUT 120)
  endif()
  add_executable(virtualcortex_nam_render tools/nam_render.cpp)
  target_link_libraries(virtualcortex_nam_render PRIVATE vc_nam_core)
  if(BUILD_TESTING AND Python3_Interpreter_FOUND)
    add_test(NAME nam_offline_compatibility COMMAND ${Python3_EXECUTABLE}
      "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_nam_renderer.py"
      $<TARGET_FILE:virtualcortex_nam_render>)
    add_test(NAME nam_upstream_lstm COMMAND virtualcortex_nam_render
      --probe "${VC_NAM_SOURCE_DIR}/example_models/lstm.nam")
    add_test(NAME nam_upstream_wavenet COMMAND virtualcortex_nam_render
      --probe "${VC_NAM_SOURCE_DIR}/example_models/wavenet.nam")
    set_tests_properties(nam_offline_compatibility nam_upstream_lstm nam_upstream_wavenet PROPERTIES TIMEOUT 120)
  endif()
endif()
