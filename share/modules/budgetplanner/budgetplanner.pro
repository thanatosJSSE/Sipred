
include(../../../sipred.pri)
include(../../../dependences.pri)

QT += core gui sql

TEMPLATE = lib
CONFIG += plugin uitools

TARGET = $$qtLibraryTarget(budgetplannermodule)
DESTDIR = $$SIPRED_MODULE_PATH/BudgetPlanner

SOURCES += \
    src/budgetplanner.cpp \
    src/controlwidget.cpp \
    src/configwidget.cpp \
    src/centralwidget.cpp \
    src/budgetplannerfactory.cpp \

HEADERS += \
    include/budgetplanner.h \
    include/controlwidget.h \
    include/configwidget.h \
    include/centralwidget.h \
    include/budgetplannerfactory.h

OTHER_FILES += \
    budgetplanner.qrc
