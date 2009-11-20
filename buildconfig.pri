MOC_DIR = $$OBJECTS_DIR
UI_DIR = $$OBJECTS_DIR
RCC_DIR = $$OBJECTS_DIR
unix:isEmpty( INSTALL_PREFIX ):INSTALL_PREFIX = /usr/local
unix: {
	SHARE_DIR = $$INSTALL_PREFIX/share/tagainijisho
	BIN_DIR = $$INSTALL_PREFIX/bin
}
macx: {
	CONFIG += x86 ppc
	LIBS += -bind_at_load
}
DEFINES += VERSION=0.2.395
unix::DEFINES += DATAPREFIX=$$SHARE_DIR
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
