CONFIG += link_pkgconfig
QT += core gui widgets
INCLUDEPATH += ../../svf
SOURCES = run-svf.cpp filter_editor.cpp ../../svf/VAStateVariableFilter.cpp
JEADERS = filter_editor.h
FORMS = filter_editor.ui
PKGCONFIG += jack
