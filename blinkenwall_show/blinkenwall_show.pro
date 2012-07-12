QT          += opengl

HEADERS     = glwidget.h \
              helper.h \
              widget.h \
              window.h \
              main.h \
              

SOURCES     = glwidget.cpp \
              helper.cpp \
              main.cpp \
              widget.cpp \
              window.cpp \
              


LIBS = $(SUBLIBS) -L/usr/lib

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/opengl/blinkenwall_show
#sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS blinkenwall_show.pro
#sources.path = $$[QT_INSTALL_EXAMPLES]/opengl/blinkenwall_show
#INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)



