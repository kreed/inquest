TEMPLATE = app

CONFIG += release warn_on embed_manifest_exe

TARGET = inquest

MOC_DIR = build
OBJECTS_DIR = build

SOURCES += src/*.cpp
HEADERS += src/*.h
