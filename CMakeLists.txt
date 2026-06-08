cmake_minimum_required(VERSION 3.16)

project(Projekt_v_1_2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Wskaż folder, w którym wypakowałeś SFML (użyj ukośników /)
set(SFML_DIR "C:/SFML/lib/cmake/SFML")

# 2. Znajdź bibliotekę SFML
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

# 3. Dodaj plik wykonywalny
add_executable(Projekt_v_1_2 main.cpp
    Game.h
    Game.cpp
    Note.h
    Note.cpp
    NoteType.h
    NoteType.cpp
    ScoreManager.h
    ScoreManager.cpp
    InputHandler.h
    InputHandler.cpp
    music_sequence/muzyka_test.txt
    Interface.h
    Interface.cpp
    "build/Desktop_Qt_6_11_1_MinGW_64_bit-Debug/music_sequence/Lady Pank.txt" build/Desktop_Qt_6_11_1_MinGW_64_bit-Debug/music_sequence/muzyka1.txt build/Desktop_Qt_6_11_1_MinGW_64_bit-Debug/music_sequence/muzyka2.txt "build/Desktop_Qt_6_11_1_MinGW_64_bit-Debug/music_sequence/Utwór 2 level 1.txt" "build/Desktop_Qt_6_11_1_MinGW_64_bit-Debug/music_sequence/Utwór 2 level 2.txt")

# 4. PRZEŁOMOWY MOMENT: Linkowanie bibliotek
# target_link_libraries łączy Twój projekt z bibliotekami SFML
target_link_libraries(Projekt_v_1_2 PRIVATE sfml-graphics sfml-window sfml-system)

include(GNUInstallDirs)
install(TARGETS Projekt_v_1_2
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)