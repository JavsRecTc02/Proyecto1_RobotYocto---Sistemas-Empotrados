SUMMARY = "Configuracion ALSA para jack de audio RPi4"
LICENSE = "CLOSED"
SRC_URI = "file://asound.conf"

do_install() {
    install -d ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/asound.conf ${D}${sysconfdir}/asound.conf
}

FILES:${PN} = "${sysconfdir}/asound.conf"
