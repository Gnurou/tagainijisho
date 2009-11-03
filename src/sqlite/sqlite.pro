include(../../buildconfig.pri)
TEMPLATE = lib
TARGET = sqlite3
CONFIG += qt staticlib
QT = core sql
INCLUDEPATH = .. ../../3rdparty/sqlite
DEFINES += SQLITE_ENABLE_FTS3
HEADERS += qsql_sqlite.h \
	../core/TextTools.h
SOURCES += sqlite3all.c \
	sqlite3ext.cc \
	qsql_sqlite.cc \
	_qsqlcachedresult.cc \
	../core/TextTools.cc
