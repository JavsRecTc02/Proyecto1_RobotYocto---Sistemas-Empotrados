SUMMARY = "Robot Vacuum HTTP Server"
LICENSE = "CLOSED"

SRC_URI = "file://src/main.c        \
           file://src/api.c         \
           file://src/api.h         \
           file://src/auth.c        \
           file://src/auth.h        \
           file://src/sha256.c      \
           file://src/sha256.h      \
           file://src/robot_state.c \
           file://src/robot_state.h \
           file://src/robot_hardware.c \
           file://src/robot_hardware.h \
	   file://src/CMakeLists.txt \
           file://www               \
           file://audio             \
           file://robot-server.service"

S = "${WORKDIR}/src"

DEPENDS = "libmicrohttpd librobot pigpio"

inherit cmake systemd

SYSTEMD_SERVICE:${PN} = "robot-server.service"
SYSTEMD_AUTO_ENABLE = "enable"

do_install:append() {
    if [ -d ${WORKDIR}/www ]; then
        install -d ${D}/opt/robot/www
        cp -r ${WORKDIR}/www/* ${D}/opt/robot/www/ || true
    fi
    if [ -d ${WORKDIR}/audio ]; then
        install -d ${D}/opt/robot/audio
        cp -r ${WORKDIR}/audio/* ${D}/opt/robot/audio/ || true
    fi
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/robot-server.service \
                    ${D}${systemd_system_unitdir}/robot-server.service
}

FILES:${PN} += "/opt/robot"
