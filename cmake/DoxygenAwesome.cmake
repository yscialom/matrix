include(FetchContent)

FetchContent_Declare(
    doxygen_awesome_css
    GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
    GIT_TAG        v2.3.4
    GIT_SHALLOW    TRUE
)

# doxygen-awesome-css has no CMakeLists.txt, so we populate without
# add_subdirectory(). CMP0169=OLD silences the FetchContent_Populate
# deprecation warning introduced in CMake 3.28 while keeping compatibility.
if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()
FetchContent_GetProperties(doxygen_awesome_css)
if(NOT doxygen_awesome_css_POPULATED)
    FetchContent_Populate(doxygen_awesome_css)
endif()

set(DOXYGEN_AWESOME_CSS_DIR "${doxygen_awesome_css_SOURCE_DIR}")

# Generate the default Doxygen HTML header and inject doxygen-awesome extension
# init calls. This must happen at configure time so configure_file() can pick up
# DOXYGEN_AWESOME_HEADER.
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc")
execute_process(
    COMMAND "${DOXYGEN_EXECUTABLE}" -w html
        "${CMAKE_CURRENT_BINARY_DIR}/doc/header_base.html"
        "${CMAKE_CURRENT_BINARY_DIR}/doc/footer_base.html"
        "${CMAKE_CURRENT_BINARY_DIR}/doc/style_base.css"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    OUTPUT_QUIET
    ERROR_QUIET
)
file(READ "${CMAKE_CURRENT_BINARY_DIR}/doc/header_base.html" _doxygen_header)
string(REPLACE "</head>" [=[
<script type="text/javascript" src="$relpath^doxygen-awesome-darkmode-toggle.js"></script>
<script type="text/javascript" src="$relpath^doxygen-awesome-fragment-copy-button.js"></script>
<script type="text/javascript" src="$relpath^doxygen-awesome-paragraph-link.js"></script>
<script type="text/javascript" src="$relpath^doxygen-awesome-interactive-toc.js"></script>
<script type="text/javascript">
    DoxygenAwesomeDarkModeToggle.init()
    DoxygenAwesomeFragmentCopyButton.init()
    DoxygenAwesomeParagraphLink.init()
    DoxygenAwesomeInteractiveToc.init()
</script>
</head>]=]
    _doxygen_header "${_doxygen_header}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/doc/header.html" "${_doxygen_header}")

set(DOXYGEN_AWESOME_HEADER "${CMAKE_CURRENT_BINARY_DIR}/doc/header.html")
