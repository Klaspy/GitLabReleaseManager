qt_add_library(CustomStyle STATIC)
set_source_files_properties(GlobalStyle.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
qt_add_qml_module(CustomStyle
    URI CustomStyle
    VERSION 1.0
    QML_FILES
        Button.qml
        ComboBox.qml
        GlobalStyle.qml
        Text.qml
    RESOURCE_PREFIX /
)
