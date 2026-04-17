require recipes-core/images/core-image-minimal.bb

SUMMARY = "Imagen Robot Vacuum para Raspberry Pi 4"

IMAGE_INSTALL:append = " \
    robot-server                        \
    libmicrohttpd                       \
    mpg123                              \
    alsa-utils                          \
    alsa-lib                            \
    alsa-plugins                        \
    alsa-config                         \
    wpa-supplicant                      \
    wpa-config                          \
    linux-firmware-rpidistro-bcm43455   \
    iw                                  \
    openssh-sftp-server                 \
"
IMAGE_FEATURES += "ssh-server-dropbear"
MACHINE_FEATURES:append = " wifi"
IMAGE_ROOTFS_SIZE ?= "122880"
IMAGE_ROOTFS_EXTRA_SPACE ?= "0"

