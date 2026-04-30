SUMMARY = "Robot hardware shared library"
LICENSE = "CLOSED"

SRC_URI = "file://lib_audio.c   \
           file://lib_audio.h   \
           file://lib_leds.c    \
           file://lib_leds.h    \
           file://lib_motors.c  \
           file://lib_motors.h  \
           file://lib_sensors.c \
           file://lib_sensors.h \
           file://robot_state.h \
           file://CMakeLists.txt"

S = "${WORKDIR}"

DEPENDS = "mpg123 alsa-lib pigpio"

inherit cmake

FILES:${PN}     = "${libdir}/librobot.so.1* "
FILES:${PN}-dev = "${libdir}/librobot.so ${includedir}/robot/*.h"
