SUMMARY = "pigpio is a C library for the Raspberry which allows control of the General Purpose Input Outputs (GPIO)."
DESCRIPTION = "pigpio is a C library for the Raspberry which allows control of the General Purpose Input Outputs (GPIO)."
SECTION = "utils"
LICENSE = "Unlicense"
LIC_FILES_CHKSUM = "file://UNLICENCE;md5=61287f92700ec1bdf13bc86d8228cd13"

SRC_URI = "git://github.com/joan2937/pigpio.git;protocol=https;branch=master \
           file://pigpiod.service "
SRCREV = "c33738a320a3e28824af7807edafda440952c05d"

S = "${WORKDIR}/git"

EXTRA_OEMAKE += "CC='${CC}'"
EXTRA_OEMAKE += "CROSS_PREFIX=${TARGET_PREFIX}"
EXTRA_OEMAKE += "STRIP=echo"
EXTRA_OEMAKE += "PYINSTALLARGS='--root=$(DESTDIR) --prefix=${prefix}'"
TARGET_CC_ARCH += "${LDFLAGS}"

ALLOW_EMPTY:${PN} = "1"
ALLOW_EMPTY:${PN}-dbg = "0"
ALLOW_EMPTY:${PN}-dev = "0"

FILES:${PN}-bin-pigs    = "${bindir}/pigs"
FILES:${PN}-bin-pig2vcd = "${bindir}/pig2vcd"
FILES:${PN}-bin-pigpiod    = "${bindir}/pigpiod"
RDEPENDS:${PN}-bin-pigpiod = "lib${PN}"

RDEPENDS:${PN}-bin = "${PN}-bin-pigpiod ${PN}-bin-pigs ${PN}-bin-pig2vcd"
ALLOW_EMPTY:${PN}-bin = "1"

FILES:lib${PN}     = "${libdir}/lib${PN}.so.*"
FILES:lib${PN}     =+ "/opt/${PN}/cgi"
FILES:lib${PN}_if  = "${libdir}/lib${PN}_if.so.*"
FILES:lib${PN}_if2 = "${libdir}/lib${PN}_if2.so.*"

FILES:${PN}-dev += "${libdir}/lib${PN}*.so"
FILES:${PN}-doc  = "${mandir}"

PACKAGES =+ " ${PN}-bin-pigpiod ${PN}-bin-pigs ${PN}-bin-pig2vcd \
              lib${PN} lib${PN}_if lib${PN}_if2"

do_install() {
    oe_runmake install DESTDIR=${D} prefix=${prefix} mandir=${mandir}

    # Eliminar archivos Python que no necesitamos
    rm -rf ${D}/usr/local

    # Instalar servicio systemd
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/pigpiod.service \
                    ${D}${systemd_system_unitdir}/pigpiod.service
}

inherit lib_package systemd
SYSTEMD_SERVICE:${PN} = "pigpiod.service"
SYSTEMD_AUTO_ENABLE = "enable"
