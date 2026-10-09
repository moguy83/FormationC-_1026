add_library(bibliotheque STATIC src/livre.cpp src/auteur.cpp src/catalogue.cpp)
# PUBLIC : les consommateurs doivent connaître le chemin des headers et C++20.
target_include_directories(bibliotheque PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(bibliotheque PUBLIC cxx_std_20)
# PRIVATE : JSON/spdlog restent des détails du .cpp, absents des headers publics.
target_link_libraries(bibliotheque PRIVATE nlohmann_json::nlohmann_json spdlog::spdlog options_cours)
add_executable(bibliotheque_app src/main.cpp)
target_link_libraries(bibliotheque_app PRIVATE bibliotheque spdlog::spdlog options_cours)
if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
if(BUILD_DOCS)
  find_package(Doxygen REQUIRED)
  configure_file(Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(docs ALL COMMAND Doxygen::doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} VERBATIM)
endif()
install(TARGETS bibliotheque_app RUNTIME DESTINATION bin)
install(TARGETS bibliotheque ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION include PATTERN catalogue_bdd.hpp EXCLUDE)
# Cette installation n'exporte pas de package CMake find_package : hors périmètre de ce TP.
