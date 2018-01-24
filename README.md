# SorpSim

This is a Sorption system simulation tool to evaluate different thermally
activated cooling and heating technologies.

# Getting started

* Please switch to the new repository [sorpsim-opt](https://github.com/nfette/sorpsim-opt).
  Explanation below.
* To download installer see the Releases link on Github
* Developers: [documentation](http://www.public.asu.edu/~nfette/SorpSim/latest/)

Note from nfette: After the release of v1.1, I am planning to discontinue 
pushing to this particular repository in order to remove some large binary
files, but may continue working in a new repository.


# Installing and Running

Look on the [releases](https://github.com/nfette/SorpSim/releases)
tab to download the latest released binaries.
You can use the self-extracting installer or the archive file appropriate for
your system. Run the installer or extract the files to the folder of your
choice, then run the executable (SorpSim).
(I may need someone to build the project on Mac OS.)

# Building

To compile this yourself you need to install these prerequisites:

* Any modern C++ compiler
* Qt version >5 (tested with 5.9.3)
* Qwt http://qwt.sourceforge.net/ (follow instructions to build and install)

Configure the project file, SorpSim.pro.

* Set the paths to find Qwt library and (optional) sources
* Note that there is a target called 'install' that just copies the extra files
  (examples, settings, etc) into the build directory. It doesn't install the program.

QtCreator is a convenient IDE for working on code that uses Qt, but it is not
required to build the code. You can also just build on the command line.

## Building with QtCreator

Building is easy.
* Open the project, SorpSim.pro.
* Choose a compiler kit and build directory (defaults usually work well) then
click the button to configure.
* Open the Projects mode, and edit the Run Settings. Add a deployment step using
make. Enter "install" (no quotes) in the extra arguments tab.
* Open the Edit mode and run from menus Build > Build project.

## Building with qmake

* qmake SorpSim.pro
* make
* make install

## Deployment

To build the installer or a binary distribution, you can use the tool provided
by Qt to recognize dependencies and prepare a bundle of files for distribution.
A recipe is found in the source folder.
See http://doc.qt.io/qt-5/deployment.html

## Special note for MSVC users

If your compiler is MSVC, you may need to take extra steps.

* To get the compiler working, some users may need to run the build session from
a command window, starting by executing a short batch file to set environment
parameters at the start of build session. More info to follow.
* To get the most out of your computer, install and use JOM as a drop-in
replacement for nmake. In QtCreator, it is already bundled, and you configure
its usage in the Projects view. On the command line, simply add it to your path.
Refer to advice in the Qwt install directions.
* If you build on command line, substitute nmake (or jom) for make

# Contributing

Please get in touch with the other authors and see if there are things to do.
