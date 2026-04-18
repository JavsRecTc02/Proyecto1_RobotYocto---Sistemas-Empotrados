SUMMARY = "Preconfigured WiFi for headless boot"
LICENSE = "CLOSED"

SRC_URI = " \
            file://wpa_supplicant-wlan0.conf \
            file://25-wlan.network \
    "

S = "${WORKDIR}"

do_install() {

    install -d ${D}${sysconfdir}/wpa_supplicant
    install -m 600 ${WORKDIR}/wpa_supplicant-wlan0.conf \
        ${D}${sysconfdir}/wpa_supplicant/

    # Install the systemd-networkd config
    install -d ${D}${sysconfdir}/systemd/network
    install -m 644 ${WORKDIR}/25-wlan.network \
        ${D}${sysconfdir}/systemd/network/

    install -d ${D}${sysconfdir}/systemd/system/multi-user.target.wants
    ln -sf /usr/lib/systemd/system/wpa_supplicant@.service \
        ${D}${sysconfdir}/systemd/system/multi-user.target.wants/wpa_supplicant@wlan0.service

        
}

FILES:${PN} += " \
    ${sysconfdir}/wpa_supplicant/wpa_supplicant-wlan0.conf \
    ${sysconfdir}/systemd/network/25-wlan.network \
    ${sysconfdir}/systemd/system/multi-user.target.wants/wpa_supplicant@wlan0.service \
"
