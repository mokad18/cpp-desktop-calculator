cmake_minimum_required(VERSION 3.31)
project(Calculator)

set(CMAKE_CXX_STANDARD 20)

# ——— FLTK via Homebrew ———
# Suche das mit Homebrew installierte fltk-config
find_program(FLTK_CONFIG fltk-config HINTS
        /opt/homebrew/bin
        /usr/local/bin
        REQUIRED
)

# Compiler- und Linker-Flags aus fltk-config holen
execute_process(
        COMMAND ${FLTK_CONFIG} --cxxflags
        OUTPUT_VARIABLE FLTK_CXXFLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
        COMMAND ${FLTK_CONFIG} --ldstaticflags
        OUTPUT_VARIABLE FLTK_LDFLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "FLTK cxxflags: ${FLTK_CXXFLAGS}")
message(STATUS "FLTK ldflags:  ${FLTK_LDFLAGS}")

# Compiler-Flags in Liste splitten (space-separiert)
separate_arguments(FLTK_CXXFLAGS_LIST UNIX_COMMAND "${FLTK_CXXFLAGS}")

#FLTK Include-Pfad extrahieren (für CLion-Indexer)
# CLion brauchtexplizite target_include_directories, sonst werden
# FLTK-Header rot unterstrichen
string(REGEX MATCH "-I[^ ]+" FLTK_INCLUDE_DIR "${FLTK_CXXFLAGS}")
string(REPLACE "-I" "" FLTK_INCLUDE_DIR "${FLTK_INCLUDE_DIR}")
message(STATUS "FLTK include dir: ${FLTK_INCLUDE_DIR}")

# Executable
add_executable(Calculator main.cpp)

# Beide Pfade: Cellar (Compiler) + Homebrew-Symlink
target_include_directories(Calculator PRIVATE
        ${FLTK_INCLUDE_DIR}
        /opt/homebrew/include
)
target_compile_options(Calculator PRIVATE ${FLTK_CXXFLAGS_LIST})
# SHELL: übergibt den gesamten String unverändert an den Linker,
# sodass -framework Cocoa korrekt erhalten bleibt
target_link_options(Calculator PRIVATE "SHELL:${FLTK_LDFLAGS}")