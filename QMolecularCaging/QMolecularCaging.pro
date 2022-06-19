#-------------------------------------------------
#
# Project created by QtCreator 2018-08-31T14:35:13
#
#-------------------------------------------------

QT       += core gui

QMAKE_CXXFLAGS += -std=c++11 -Wno-sign-compare

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets 3dcore 3drender 3dinput 3dextras

TARGET = QMolecularCaging
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /usr/include/openbabel3 /usr/local/include /usr/include

LIBS += -L/usr/lib -L/usr/local/lib/x86_64-linux-gnu -lopenbabel -lCGAL -lCGAL_Core -lgmp -lmpfr

SOURCES += \
    main.cpp \
    ui/main_window.cpp \
    ui/widgets/custom_tab_bar.cpp \
    ui/widgets/custom_tab_widget.cpp \
    ui/tabs/file_tab.cpp \
    chemistry/molecule.cpp \
    utils/file_utilities.cpp \
    chemistry/molecule_collection.cpp \
    chemistry/element_collection.cpp \
    ui/tabs/experiment_tab.cpp \
    ui/widgets/custom_list_widget.cpp \
    caging3d/ballscollisionspace.cpp \
    caging3d/ballsconnectivitygraphvertex.cpp \
    caging3d/ballsunion.cpp \
    caging3d/cagingjob.cpp \
    caging3d/dualcircles.cpp \
    caging3d/intersections.cpp \
    caging3d/object3d.cpp \
    caging3d/obstacle.cpp \
    caging3d/quaternion.cpp \
    caging3d/slice.cpp \
    caging_job_thread.cpp \
    ui/tabs/analysis_tab.cpp \
    ui/models/summary_table_model.cpp \
    utils/caging_utilities.cpp \
    ui/tabs/visualization_tab.cpp \
    utils/qt_3d_utilities.cpp \
    caging_job_single_process_thread.cpp \
    caging_job_multi_process_thread.cpp

HEADERS += \
    ui/main_window.h \
    ui/widgets/custom_tab_bar.h \
    ui/widgets/custom_tab_widget.h \
    ui/tabs/file_tab.h \
    chemistry/molecule.h \
    utils/file_utilities.h \
    chemistry/molecule_collection.h \
    chemistry/element_collection.h \
    ui/tabs/experiment_tab.h \
    ui/widgets/custom_list_widget.h \
    caging3d/ballscollisionspace.h \
    caging3d/ballsconnectivitygraphvertex.h \
    caging3d/ballsunion.h \
    caging3d/caging_includes.h \
    caging3d/cagingjob.h \
    caging3d/cgal_includes.h \
    caging3d/dualcircles.h \
    caging3d/includes.h \
    caging3d/intersections.h \
    caging3d/object3d.h \
    caging3d/obstacle.h \
    caging3d/quaternion.h \
    caging3d/slice.h \
    caging_job_thread.h \
    ui/tabs/analysis_tab.h \
    ui/models/summary_table_model.h \
    utils/caging_utilities.h \
    ui/tabs/visualization_tab.h \
    utils/qt_3d_utilities.h \
    caging_job_single_process_thread.h \
    caging_job_multi_process_thread.h

FORMS += \
    ui/main_window.ui \
    ui/tabs/file_tab.ui \
    ui/tabs/experiment_tab.ui \
    ui/tabs/analysis_tab.ui \
    ui/tabs/visualization_tab.ui
