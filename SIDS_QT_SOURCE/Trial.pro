#-------------------------------------------------
#
# Project created by QtCreator 2019-03-03T00:44:18
#
#-------------------------------------------------

QT       += core gui sql network

INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_videoio -lopencv_imgproc -lopencv_dnn -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann
#LIBRARIES += opencv_core opencv_highgui opencv_imgproc opencv_video
#LIBS += opencv_core opencv_highgui opencv_imgproc opencv_videoio

QMAKE_CXXFLAGS += -std=c++11 -ggdb `pkg-config --cflags --libs opencv`

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sids_stack
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
DEFINES += SMTP_BUILD

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        smtp_client/src/emailaddress.cpp \
        smtp_client/src/mimeattachment.cpp \
        smtp_client/src/mimefile.cpp \
        smtp_client/src/mimehtml.cpp \
        smtp_client/src/mimeinlinefile.cpp \
        smtp_client/src/mimemessage.cpp \
        smtp_client/src/mimepart.cpp \
        smtp_client/src/mimetext.cpp \
        smtp_client/src/smtpclient.cpp \
        smtp_client/src/quotedprintable.cpp \
        smtp_client/src/mimemultipart.cpp \
        smtp_client/src/mimecontentformatter.cpp \

HEADERS += \
        mainwindow.h \
        smtp_client/src/emailaddress.h \
        smtp_client/src/mimeattachment.h \
        smtp_client/src/mimefile.h \
        smtp_client/src/mimehtml.h \
        smtp_client/src/mimeinlinefile.h \
        smtp_client/src/mimemessage.h \
        smtp_client/src/mimepart.h \
        smtp_client/src/mimetext.h \
        smtp_client/src/smtpclient.h \
        smtp_client/src/SmtpMime \
        smtp_client/src/quotedprintable.h \
        smtp_client/src/mimemultipart.h \
        smtp_client/src/mimecontentformatter.h \
        smtp_client/src/smtpexports.h


FORMS += \
        mainwindow.ui

RESOURCES += \
    images.qrc

#CONFIG += console
