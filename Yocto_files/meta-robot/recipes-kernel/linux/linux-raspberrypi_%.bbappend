FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://wifi.cfg"

KERNEL_CONFIG_FRAGMENTS += "${WORKDIR}/wifi.cfg"
