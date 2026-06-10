cmake_minimum_required(VERSION 3.16)

project(Projekt_v_1_2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Wskaż folder, w którym wypakowałeś SFML (użyj ukośników /)
set(SFML_DIR "C:/SFML-2.6.2/lib/cmake/SFML")

# 2. Znajdź bibliotekę SFML wraz z modułem audio
find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

# 3. Dodaj plik wykonywalny (Tylko pliki źródłowe kodu źródłowego .cpp oraz .h)
add_executable(Projekt_v_1_2
    main.cpp
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
    Interface.h
    Interface.cpp
)

# 4. Linkowanie bibliotek (Musi być PO instrukcji add_executable!)
target_link_libraries(Projekt_v_1_2 PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

# 5. Instalacja (opcjonalna konfiguracja)
include(GNUInstallDirs)
install(TARGETS Projekt_v_1_2
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)