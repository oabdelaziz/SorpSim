#-------------------------------------------------
#
# Project created by QtCreator 2013-10-18T10:52:03
#
#-------------------------------------------------

QT       += core gui
QT       += xml
QT       += printsupport

CONFIG   += qwt

INCLUDEPATH += $$(QWTPATH)/src
DEPENDPATH += $$(QWTPATH)/src

#LIBS += C:\qwt-6.1.0\lib\qwt.dll
win32:CONFIG(release, debug|release): LIBS += -L$$(QWTPATH)/lib/ -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$(QWTPATH)/lib/ -lqwtd
else:unix: LIBS += -L$$(QWTPATH)/lib/ -lqwt

#include(c:/qwt-6.1.0/qwt.prf)
#####include the qwt library for Mac compilation
#####directory might be different for different machine settings

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SorpSim
TEMPLATE = app

CONFIG(debug, debug|release) {TARGET_PATH = $$OUT_PWD/debug}
CONFIG(release, debug|release) {TARGET_PATH = $$OUT_PWD/release}
mythinga.path = $$TARGET_PATH/platforms
mythinga.files = platforms/*
mythingb.path = $$TARGET_PATH/templates
mythingb.files = templates/*
INSTALLS += mythinga mythingb

SOURCES += main.cpp \
    unitconvert.cpp \
    unit.cpp \
    treedialog.cpp \
    tableselectparadialog.cpp \
    tabledialog.cpp \
    spscene.cpp \
    spdialog.cpp \
    selectparadialog.cpp \
    resultdisplaydialog.cpp \
    resultdialog.cpp \
    plotproperty.cpp \
    node.cpp \
    myview.cpp \
    myscene.cpp \
    mainwindow.cpp \
    linkdialog.cpp \
    link.cpp \
    globaldialog.cpp \
    fluiddialog.cpp \
    editunitdialog.cpp \
    calculate.cpp \
    arrow.cpp \
    altervdialog.cpp \
    adrowdialog.cpp \
    masterdialog.cpp \
    masterpanelcell.cpp \
    syssettingdialog.cpp \
    guessdialog.cpp \
    airarrow.cpp \
    zigzag.cpp \
    packings.cpp \
    coils.cpp \
    ldaccompdialog.cpp \
    splitterdialog.cpp \
    ntuestimatedialog.cpp \
    startdialog.cpp \
    valvedialog.cpp \
    pumpdialog.cpp \
    qgraphicsarc.cpp \
    vicheckdialog.cpp \
    iwfixdialog.cpp \
    ipfixdialog.cpp \
    iffixdialog.cpp \
    icfixdialog.cpp \
    dataComm.cpp \
    insidelink.cpp \
    dehumeffdialog.cpp \
    estntueffdialog.cpp \
    linkfluiddialog.cpp \
    newparaplotdialog.cpp \
    plotsdialog.cpp \
    newpropplotdialog.cpp \
    calcdetaildialog.cpp \
    edittabledialog.cpp \
    helpdialog.cpp \
    sorpsimEngine.cpp \
    overlaysettingdialog.cpp \
    texteditdialog.cpp \
    unitsettingdialog.cpp \
    curvesettingdialog.cpp \
    editpropertycurvedialog.cpp \
    ifixdialog.cpp

HEADERS  += \
    unitconvert.h \
    unit.h \
    treedialog.h \
    tableselectparadialog.h \
    tabledialog.h \
    spscene.h \
    spdialog.h \
    selectparadialog.h \
    resultdisplaydialog.h \
    resultdialog.h \
    plotproperty.h \
    pixmap.h \
    node.h \
    myview.h \
    myscene.h \
    mainwindow.h \
    linkdialog.h \
    link.h \
    globaldialog.h \
    fluiddialog.h \
    editunitdialog.h \
    calculate.h \
    arrow.h \
    altervdialog.h \
    adrowdialog.h \
    masterdialog.h \
    masterpanelcell.h \
    dataComm.h \
    fem.hpp \
    syssettingdialog.h \
    guessdialog.h \
    airarrow.h \
    zigzag.h \
    packings.h \
    coils.h \
    ldaccompdialog.h \
    splitterdialog.h \
    ntuestimatedialog.h \
    startdialog.h \
    valvedialog.h \
    pumpdialog.h \
    qgraphicsarc.h \
    vicheckdialog.h \
    iwfixdialog.h \
    ipfixdialog.h \
    iffixdialog.h \
    icfixdialog.h \
    insidelink.h \
    dehumeffdialog.h \
    estntueffdialog.h \
    linkfluiddialog.h \
    newparaplotdialog.h \
    plotsdialog.h \
    newpropplotdialog.h \
    calcdetaildialog.h \
    editpropertycurvedialog.h \
    edittabledialog.h \
    helpdialog.h \
    sorpsimEngine.h \
    overlaysettingdialog.h \
    texteditdialog.h \
    unitsettingdialog.h \
    curvesettingdialog.h \
    ifixdialog.h

FORMS    += \
    treedialog.ui \
    tableselectparadialog.ui \
    tabledialog.ui \
    spdialog.ui \
    selectparadialog.ui \
    resultdisplaydialog.ui \
    resultdialog.ui \
    mainwindow.ui \
    linkdialog.ui \
    globaldialog.ui \
    fluiddialog.ui \
    editUnitDialog.ui \
    altervdialog.ui \
    adrowdialog.ui \
    masterdialog.ui \
    syssettingdialog.ui \
    guessdialog.ui \
    ldaccompdialog.ui \
    splitterdialog.ui \
    ntuestimatedialog.ui \
    startdialog.ui \
    valvedialog.ui \
    pumpdialog.ui \
    vicheckdialog.ui \
    iwfixdialog.ui \
    ipfixdialog.ui \
    iffixdialog.ui \
    icfixdialog.ui \
    dehumeffdialog.ui \
    estntueffdialog.ui \
    linkfluiddialog.ui \
    newparaplotdialog.ui \
    plotsdialog.ui \
    newpropplotdialog.ui \
    calcdetaildialog.ui \
    editpropertycurvedialog.ui \
    edittabledialog.ui \
    helpdialog.ui \
    overlaysettingdialog.ui \
    texteditdialog.ui \
    unitsettingdialog.ui \
    curvesettingdialog.ui \
    ifixdialog.ui

RESOURCES += \
    functionIcons.qrc
