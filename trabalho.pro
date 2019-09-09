QT       += core
QT       -= gui

TARGET = OpenCV-Ex1
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11
TEMPLATE = app

SOURCES += main.cpp

win32 {
    INCLUDEPATH += C:/opencv-build-3.4.3/install/include

    LIBS += -LC:/opencv-build-3.4.3/install/x86/mingw/lib \
        -llibopencv_imgcodecs343 \
        -llibopencv_core343 \
        -llibopencv_highgui343 \
        -llibopencv_imgproc343 \
        -llibopencv_features2d343 \
        -llibopencv_calib3d343 \
        -llibopencv_ml343 \
        -llibopencv_features2d343 \
        -llibopencv_videoio343
}

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv

}
