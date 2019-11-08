#-------------------------------------------------
#
# Project created by QtCreator 2013-10-18T10:52:03
#
#-------------------------------------------------

QT       += core gui
QT       += xml
QT       += printsupport

CONFIG += c++11
win32:CONFIG += console

DEPENDPATH += C:\Users\ZzzYao\Documents\GitHub\f3hx\f3hx
INCLUDEPATH += C:\Users\ZzzYao\Documents\GitHub\f3hx\f3hx
LIBS += -LC:\Users\ZzzYao\Documents\GitHub\f3hx\build-f3hx-Desktop_Qt_5_8_0_MinGW_32bit-Release\release -lf3hx

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Usage: make
# Then you get sorpsim-opt[.exe]
TARGET = sorpsim-opt
TEMPLATE = app

#----------------------------------------------
# Conveniences for building to debug and deploy
#----------------------------------------------

defineReplace(sorpVersion){
    SORPVERSION = $$system("git describe --tags")
    SORPVERSION_DEFINE = \\\"$$quote($$SORPVERSION)\\\"
    message($$SORPVERSION_DEFINE)
    export(SORPVERSION)
    return($$SORPVERSION_DEFINE)
}

DEFINES += SORPVERSION=$$sorpVersion()

# https://stackoverflow.com/questions/3984104/qmake-how-to-copy-a-file-to-the-output
# Copies the given files to the destination directory
defineReplace(copySafe){
    files = $$1
    DEST = $$2

    for(FILE, files) {

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DEST ~= s,/,\\,g

        CMD += $$QMAKE_COPY $$quote($$FILE) $$quote($$DEST) $$escape_expand(\\n\\t)
    }

    return($$CMD)
}

# Usage: make install
# This copies the extra resource files (eg *.xml) to the build directory
# but does not really install the program anywhere.
# There is probably another way, to avoid the confusion.

# Note regarding target path: folder structures are different on windows by default
# See https://bugreports.qt.io/browse/QTBUG-52347
# CONFIG -= debug_and_release
TARGET_PATH = $$OUT_PWD
win32:CONFIG(debug, debug|release) {TARGET_PATH = $$OUT_PWD/debug}
win32:CONFIG(release, debug|release) {TARGET_PATH = $$OUT_PWD/release}
mythinga.path = $$TARGET_PATH/settings
mythinga.files = settings/*
mythingb.path = $$TARGET_PATH/templates
mythingb.files = templates/*
INSTALLS += mythinga \
#    mythingb

win32 {
# Usage(Windows only): make deploy
# Drops needed Qt libraries into the build directory so you can run without IDE.
deployall.target = deploy
deployall.depends = deployqwt deployqt
WINDEPLOY = windeployqt.exe
deployqt.commands = $$WINDEPLOY $$TARGET_PATH -opengl
# If something goes wrong, probably you need these:
#-printsupport -svg -xml -opengl -widgets -gui --angle -concurrent -core
CONFIG(debug, debug|release){ qwtdll = $$(QWTPATH)/lib/qwtd.dll }
CONFIG(release, debug|release){ qwtdll = $$(QWTPATH)/lib/qwt.dll }
deployqwt.commands = $$copySafe($$qwtdll, $$TARGET_PATH)

QMAKE_EXTRA_TARGETS += deployall deployqt deployqwt
}

macx {
BUNDLE_NAME = $${TARGET}.app
BUNDLE_PATH = $$OUT_PWD/$$BUNDLE_NAME
EXE_PATH = $$BUNDLE_PATH/Contents/MacOS/$$TARGET
QWT_DEST = $$BUNDLE_PATH/Contents/Frameworks/qwt.framework
# Usage: make deploy
# - Installs qwt.framework in the bundle (deployqwt)
# - Runs the Qt deployment tool (deployqt)
# - Edits the Info.plist file for "Get info" dialogs (deployinfo)
deployall.target = deploy
deployall.depends = deployqwt deployqt deployinfo
MACDEPLOY = macdeployqt
deployqt.depends = deployqwt
deployqt.commands = $$MACDEPLOY $$BUNDLE_PATH
deployqwt.depends = deployqwt1 deployqwt2
deployqwt1.target = $$BUNDLE_NAME/Contents/Frameworks/qwt.framework/qwt
deployqwt1.commands = test -d $$BUNDLE_PATH/Contents/Frameworks || mkdir -p $$BUNDLE_PATH/Contents/Frameworks $$escape_expand(\\n\\t)
deployqwt1.commands += $$QMAKE_DEL_TREE $$QWT_DEST $$escape_expand(\\n\\t)
deployqwt1.commands += $$QMAKE_COPY_DIR $$QWT_INSTALL_LIBS/qwt.framework $$QWT_DEST
deployqwt2.depends = $(TARGET)
deployqwt2.commands = install_name_tool -change qwt.framework/Versions/6/qwt @executable_path/../Frameworks/qwt.framework/Versions/6/qwt $$EXE_PATH
deployinfo.depends = $(TARGET)
deployinfo.commands = defaults write $$BUNDLE_PATH/Contents/Info.plist \"CFBundleGetInfoString\" \'$$SORPVERSION\' $$escape_expand(\\n\\t)
deployinfo.commands += defaults write $$BUNDLE_PATH/Contents/Info.plist \"CFBundleIdentifier\" \'info.nfette.sorpsim-opt\'

QMAKE_EXTRA_TARGETS += deployall deployqt deployqwt deployqwt1 deployqwt2 deployinfo

# This goes with something else above ... need to improve structure
macx:mythinga.path = $$BUNDLE_PATH/Contents/Resources/settings
}

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
    calcdetaildialog.cpp \
    edittabledialog.cpp \
    helpdialog.cpp \
    sorpsimEngine.cpp \
    texteditdialog.cpp \
    unitsettingdialog.cpp \
    editpropertycurvedialog.cpp \
    ifixdialog.cpp \
    sorputils.cpp

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
    calcdetaildialog.h \
    edittabledialog.h \
    helpdialog.h \
    sorpsimEngine.h \
    texteditdialog.h \
    unitsettingdialog.h \
    ifixdialog.h \
    sorputils.h \
    version.h \
    ../../SorpPropBatch/release build/sorpproplib.h \
    ../../SorpPropBatch/release build/sorpproplib_global.h \
    testlib.h \
    testlib_global.h \
    ../../f3hx/f3hx/f3hx.h \
    ../../f3hx/f3hx/f3hx_global.h \
    f3hx.h \
    f3hx_global.h

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
    plotsdialog.ui \
    calcdetaildialog.ui \
    editpropertycurvedialog.ui \
    edittabledialog.ui \
    helpdialog.ui \
    texteditdialog.ui \
    unitsettingdialog.ui \
    ifixdialog.ui

RESOURCES += \
    functionIcons.qrc \
    examples.qrc
