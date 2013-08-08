APP_NAME = Writer
DEFINES += QUAZIP_STATIC
DEFINES += QT_NO_WARNING_OUTPUT QT_NO_DEBUG_OUTPUT

CONFIG += qt warn_on cascades10
LIBS += -lbbdevice -lbb -lbbsystem -lbbcascadespickers

include(config.pri)

device {
    CONFIG(debug, debug|release) {
        # Device-Debug custom configuration
    }

    CONFIG(release, debug|release) {
        # Device-Release custom configuration
    }
}

simulator {
    CONFIG(debug, debug|release) {
        # Simulator-Debug custom configuration
    }
}
