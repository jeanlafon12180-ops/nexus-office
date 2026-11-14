cmake_minimum_required(VERSION 3.21)

project(NexusOffice
    VERSION 0.2.0
    DESCRIPTION "A modern cross-platform office suite"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(NEXUS_ENABLE_LIBREOFFICEKIT
    "Enable the optional LibreOfficeKit dependency (integration is not implemented yet)"
    OFF
)

find_package(Qt6 6.2 REQUIRED COMPONENTS Widgets PrintSupport)

qt_standard_project_setup()

qt_add_executable(nexus-office
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/writer/writerpage.cpp
    src/writer/writerpage.h
    src/sheets/sheetpage.cpp
    src/sheets/sheetpage.h
    src/slides/slidespage.cpp
    src/slides/slidespage.h
    src/pdf/pdfpage.cpp
    src/pdf/pdfpage.h
    src/document/documentengine.h
    src/document/unavailabledocumentengine.h
    src/document/unavailabledocumentengine.cpp
)

target_link_libraries(nexus-office PRIVATE Qt6::Widgets Qt6::PrintSupport)
target_compile_definitions(nexus-office PRIVATE NEXUS_SLIDES_ODP_HELPER="${CMAKE_SOURCE_DIR}/tools/slides_odp.py")
install(FILES tools/slides_odp.py DESTINATION share/nexus-office)

if(NEXUS_ENABLE_LIBREOFFICEKIT)
    find_path(LIBREOFFICEKIT_INCLUDE_DIR
        NAMES LibreOfficeKit/LibreOfficeKit.h
        DOC "Directory containing the LibreOfficeKit headers"
    )
    find_library(LIBREOFFICEKIT_LIBRARY
        NAMES libreofficekit
        DOC "LibreOfficeKit library"
    )

    if(NOT LIBREOFFICEKIT_INCLUDE_DIR OR NOT LIBREOFFICEKIT_LIBRARY)
        message(FATAL_ERROR
            "NEXUS_ENABLE_LIBREOFFICEKIT is ON, but LibreOfficeKit headers "
            "or library were not found. Install the matching development package "
            "for your platform, or configure with -DNEXUS_ENABLE_LIBREOFFICEKIT=OFF."
        )
    endif()

    target_include_directories(nexus-office PRIVATE ${LIBREOFFICEKIT_INCLUDE_DIR})
    target_link_libraries(nexus-office PRIVATE ${LIBREOFFICEKIT_LIBRARY})
    target_compile_definitions(nexus-office PRIVATE NEXUS_HAS_LIBREOFFICEKIT=1)
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(documentengine-tests
        tests/documentengine_test.cpp
        src/document/unavailabledocumentengine.cpp
        src/document/documentengine.h
        src/document/unavailabledocumentengine.h
    )
    add_test(NAME documentengine-contract COMMAND documentengine-tests)

    add_executable(formulaengine-tests tests/formulaengine_test.cpp)
    target_link_libraries(formulaengine-tests PRIVATE Qt6::Core)
    add_test(NAME sheets-formula-engine COMMAND formulaengine-tests)

    find_program(LIBREOFFICE_EXECUTABLE NAMES soffice)
    if(LIBREOFFICE_EXECUTABLE)
        add_test(NAME writer-docx-roundtrip
            COMMAND bash ${CMAKE_SOURCE_DIR}/tests/writer_docx_roundtrip.sh ${LIBREOFFICE_EXECUTABLE})
    endif()

    if(LIBREOFFICE_EXECUTABLE)
        add_test(NAME slides-pptx-roundtrip
            COMMAND bash ${CMAKE_SOURCE_DIR}/tests/slides_pptx_roundtrip.sh ${LIBREOFFICE_EXECUTABLE})
    endif()
endif()

set_target_properties(nexus-office PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
