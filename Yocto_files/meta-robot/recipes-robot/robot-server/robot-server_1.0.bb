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
           file://lib/lib_audio.c   \
           file://lib/lib_audio.h   \
           file://lib/lib_leds.c    \
           file://lib/lib_leds.h    \
           file://www               \
           file://audio             \
           file://robot-server.service "

S = "${WORKDIR}"

DEPENDS = "libmicrohttpd mpg123 alsa-lib"
RDEPENDS:${PN} = "mpg123 alsa-utils alsa-lib"

inherit systemd
SYSTEMD_SERVICE:${PN} = "robot-server.service"
SYSTEMD_AUTO_ENABLE = "enable"

do_compile() {
    ${CC} ${CFLAGS} \
        ${S}/src/main.c         \
        ${S}/src/api.c          \
        ${S}/src/auth.c         \
        ${S}/src/sha256.c       \
        ${S}/src/robot_state.c  \
        ${S}/lib/lib_audio.c    \
        ${S}/lib/lib_leds.c     \
        -I${S}/src              \
        -I${S}/lib              \
        -I${STAGING_INCDIR}     \
        -L${STAGING_LIBDIR}     \
        -lmicrohttpd -lpthread -lmpg123 -lasound \
        ${LDFLAGS}              \
        -o robot-server
}

do_install() {
    install -d ${D}${bindir}
    install -d ${D}/opt/robot/www
    install -d ${D}/opt/robot/audio

    install -m 0755 ${S}/robot-server ${D}${bindir}/robot-server

    if [ -d ${WORKDIR}/www ]; then
        cp -r ${WORKDIR}/www/* ${D}/opt/robot/www/ || true
    fi

    if [ -d ${WORKDIR}/audio ]; then
        cp -r ${WORKDIR}/audio/* ${D}/opt/robot/audio/ || true
    fi

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/robot-server.service \
                    ${D}${systemd_system_unitdir}/robot-server.service
}

FILES:${PN} += "/opt/robot"
