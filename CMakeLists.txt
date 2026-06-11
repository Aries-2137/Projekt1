cmake_minimum_required(VERSION 3.16)

project(Projekt_v_1_2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "C:/SFML/lib/cmake/SFML")

find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

add_executable(Projekt_v_1_2
    src/main.cpp
    src/Game.h
    src/Game.cpp
    src/Note.h
    src/Note.cpp
    src/NoteType.h
    src/NoteType.cpp
    src/ScoreManager.h
    src/ScoreManager.cpp
    src/InputHandler.h
    src/InputHandler.cpp
    src/Interface.h
    src/Interface.cpp
    src/conductor.h 
    src/conductor.cpp
)

target_link_libraries(Projekt_v_1_2 PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Kopiowanie folderu assets do katalogu uruchomieniowego..."
)

include(GNUInstallDirs)
install(TARGETS Projekt_v_1_2
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)