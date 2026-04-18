require recipes-core/images/core-image-minimal.bb

SUMMARY = "Imagen Robot Vacuum para Raspberry Pi 4"

IMAGE_INSTALL:append = " \
    robot-server                        \
    libmicrohttpd                       \
    mpg123                              \
    alsa-utils                          \
    alsa-config                         \
    wpa-supplicant                      \
    wifi-config                         \
    wireless-regdb-static               \
    linux-firmware-rpidistro-bcm43455   \
    iw                                  \
    rfkill                              \
    dhcpcd                              \
    openssh-sftp-server                 \
    kernel-module-brcmfmac-6.6.63-v8       \
    kernel-module-brcmfmac-wcc-6.6.63-v8   \
    kernel-module-brcmutil-6.6.63-v8       \
    kernel-module-snd-6.6.63-v8            \
    kernel-module-snd-pcm-6.6.63-v8        \
    kernel-module-snd-bcm2835-6.6.63-v8    \
"

IMAGE_FEATURES += "ssh-server-openssh"

IMAGE_ROOTFS_SIZE ?= "122880"
IMAGE_ROOTFS_EXTRA_SPACE ?= "0"
