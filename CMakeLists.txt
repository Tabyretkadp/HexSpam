cmake_minimum_required(VERSION 3.19)
project(TEMP_HexSpam LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Td 1.8.50)

qt_standard_project_setup()

qt_add_executable(TEMP_HexSpam
    WIN32 MACOSX_BUNDLE
    main.cpp
    main/mainwindow.cpp
    main/include/mainwindow.h

    main/include/tdapp.h
    main/tdapp.cpp

    main/mainwindow.ui

    src/include/authinput.h
    src/authinput.cpp

    src/include/core.h
    src/core.cpp
)

target_link_libraries(TEMP_HexSpam
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
        Td::TdStatic
)
target_link_libraries(TEMP_HexSpam PRIVATE Qt6::Core)

set_property(TARGET TEMP_HexSpam PROPERTY CXX_STANDARD 17)

include(GNUInstallDirs)

install(TARGETS TEMP_HexSpam
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET TEMP_HexSpam
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
