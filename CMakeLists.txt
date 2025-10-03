cmake_minimum_required(VERSION 3.10)
project(CornersGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Укажи путь по умолчанию для SFML
if(NOT DEFINED SFML_DIR)
    set(SFML_DIR "C:/SFML/lib/cmake/SFML")
endif()

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

# # Копируем фон в папку сборки
# configure_file(${CMAKE_SOURCE_DIR}/menu_background.png
               # ${CMAKE_BINARY_DIR}/menu_background.png
               # COPYONLY)


# Добавляем оба исходника
add_executable(corners_sfml WIN32 main.cpp ai.cpp)
# Копируем фон доски рядом с exe


target_link_libraries(corners_sfml sfml-graphics sfml-window sfml-system)

# Путь к SFML DLL-файлам
set(SFML_DLL_DIR "C:/SFML/bin")
set(FONT_FILE "${CMAKE_SOURCE_DIR}/DejaVuSans-Bold.ttf")

# Копировать DLL-файлы и шрифт рядом с .exe после сборки
add_custom_command(TARGET corners_sfml POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SFML_DLL_DIR}/sfml-graphics-d-2.dll
        ${SFML_DLL_DIR}/sfml-window-d-2.dll
        ${SFML_DLL_DIR}/sfml-system-d-2.dll
        ${FONT_FILE}
		${CMAKE_SOURCE_DIR}/bg.png
        $<TARGET_FILE_DIR:corners_sfml>
)

# Копируем фон в папку сборки рядом с exe
add_custom_command(TARGET corners_sfml POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/bg.png
        $<TARGET_FILE_DIR:corners_sfml>
)

add_executable(test_board test_board.cpp)

# подключаем текущую папку, где лежит doctest.h
target_include_directories(test_board PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()
add_test(NAME BoardTests COMMAND test_board)