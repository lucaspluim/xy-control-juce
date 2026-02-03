cmake_minimum_required(VERSION 3.15)
project(XYControl VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

# Fetch JUCE
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
)
FetchContent_MakeAvailable(JUCE)

# Create binary data from Resources folder
juce_add_binary_data(GlowResources
    SOURCES
        Resources/glow_layer_0.png
        Resources/glow_layer_1.png
        Resources/glow_layer_2.png
        Resources/glow_layer_3.png
        Resources/glow_layer_4.png
)

# Create the GUI application
juce_add_gui_app(XYControl
    PRODUCT_NAME "XY Control"
    COMPANY_NAME "YourCompany"
)

target_sources(XYControl PRIVATE
    Source/Main.cpp
    Source/MainComponent.cpp
    Source/XYControlComponent.cpp
)

target_compile_definitions(XYControl PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:XYControl,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:XYControl,JUCE_VERSION>"
)

target_link_libraries(XYControl PRIVATE
    juce::juce_gui_extra
    juce::juce_graphics
    GlowResources
)

# Set macOS specific properties
if(APPLE)
    set_target_properties(XYControl PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER com.yourcompany.xycontrol
    )
endif()

# Utility to generate glow images (run once during development)
add_executable(GenerateGlowImages GenerateGlowImages.cpp)
target_link_libraries(GenerateGlowImages PRIVATE
    juce::juce_graphics
    juce::juce_core
)
