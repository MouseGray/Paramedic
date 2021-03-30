QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    configsystem.cpp \
    database.cpp \
    doctor.cpp \
    main.cpp \
    mainwindow.cpp \
    patient.cpp \
    person.cpp

HEADERS += \
    Declaration.h \
    configsystem.h \
    database.h \
    doctor.h \
    main.h \
    mainwindow.h \
    patient.h \
    person.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
