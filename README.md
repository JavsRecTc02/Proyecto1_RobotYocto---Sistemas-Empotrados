# Proyecto1_RobotYocto---Sistemas-Empotrados
Repositorio dedicado al proyecto 1 del curso de Sistemas Empotrados IS2026

### Hardware Necesario

- Raspberry Pi 4 Model B
- Tarjeta microSD 
- Cable de audio Jack 3.5 mm

### Software Necesario

- Sistema operativo host basado en Linux (recomendado Ubuntu)
- Yocto Project
- Poky
- Scarthgap Release
- `bmaptool` (para grabar la imagen en la microSD)

# Layer necesarios para la imagen

Se debe tener bblayers asociados a la imagen, especialmente meta-raspberrypi y meta-openembedded para que todo el Wifi y el jack funcionen correctamente.

```bash

# POKY_BBLAYERS_CONF_VERSION is increased each time build/conf/bblayers.conf
# changes incompatibly
POKY_BBLAYERS_CONF_VERSION = "2"

BBPATH = "${TOPDIR}"
BBFILES ?= ""

BBLAYERS ?= " \
  /media/javier/disco-externo/yocto/poky-scarthgap/meta \
  /media/javier/disco-externo/yocto/poky-scarthgap/meta-poky \
  /media/javier/disco-externo/yocto/poky-scarthgap/meta-yocto-bsp \
  /media/javier/disco-externo/yocto/poky-scarthgap/meta-raspberrypi \
  /media/javier/disco-externo/yocto/poky-scarthgap/meta-openembedded/meta-oe \
  /media/javier/disco-externo/yocto/poky-scarthgap/meta-openembedded/meta-python \
  /media/javier/disco-externo/yocto/poky-scarthgap/meta-openembedded/meta-multimedia \
  /media/javier/disco-externo/yocto/poky-scarthgap/meta-openembedded/meta-networking \
  /media/javier/disco-externo/yocto/poky-scarthgap/meta-robot \
  "

```

# Archivo local.conf

En el archivo de configuracion no es recomendable incluir modulos de kernel especificos, sino solo dependencias,licencias, configuraciones, parametros, etc. Como por ejemplo bibliotecas u otros servicios de inicializacion como systemd, archivo txt de audio, parametros habilitados y demas.

```bash

# ─────────────────────────────────────────────
#  Yocto local.conf – Raspberry Pi 4 Model B
# ─────────────────────────────────────────────
CONF_VERSION = "2"
MACHINE      = "raspberrypi4-64"
DISTRO       = "poky"

# ── Init manager ──────────────────────────────
DISTRO_FEATURES:append = " systemd usrmerge wifi"
DISTRO_FEATURES_BACKFILL_CONSIDERED += "sysvinit"
VIRTUAL-RUNTIME_init_manager = "systemd"
VIRTUAL-RUNTIME_initscripts  = "systemd-compat-units"

# ── Machine features ──────────────────────────
MACHINE_FEATURES:append = " wifi alsa"

# ── Licenses ─────────────────────────────────
LICENSE_FLAGS_ACCEPTED = "synaptics-killswitch"

# ── WiFi + Audio  ───────────────────────────
ENABLE_WIFI  = "1"
ENABLE_UART  = "1"
ENABLE_AUDIO = "1"
DISABLE_VC4GRAPHICS = "1"

KERNEL_MODULE_AUTOLOAD += "cfg80211 brcmfmac brcmfmac-wcc brcmutil rfkill snd-bcm2835"

MACHINE_EXTRA_RRECOMMENDS += " \
    linux-firmware-rpidistro-bcm43455 \
    linux-firmware-rpidistro-bcm43456 \
"

# ── Audio config.txt ──────────────────────────
RPI_EXTRA_CONFIG:append = "\ndtparam=audio=on\naudio_pwm_mode=2\n"

# ── Extra - Optimization ──────────────────────────────
EXTRA_IMAGE_FEATURES += "debug-tweaks"
INHERIT += "rm_work"
RM_WORK_EXCLUDE += "robot-image"

```
# Receta robot-yocto.bb

En este archivo es importante tener en cuenta todas las bibliotecas que se utilizan en el servidor, ademas lo mas importante es agregar los kernel-modules especificos a la imagen, para esto se deben buscar los paquetes especificos dependiendo de la version de uso, para este caso la version 6.6.63-v8. Se puede hacer uso de los siguientes comandos para buscar los paquetes de interes:

## Verificar el nombre exacto de los modulos de kernel en Yocto

Por ejemplo el kernel-module-brcmfmac, ya que este puede estar incluido en IMAGE_INSTALL, pero no necesariamente carga el modulo brcmfmac.ko, el cual es el driver necesario para el servicio de Wifi.

```bash

bitbake -e virtual/kernel | grep "^PACKAGES"
oe-pkgdata-util list-pkgs | grep brcm

```
Resultado obtenido:

```bash

PACKAGES="kernel kernel-base kernel-vmlinux kernel-image kernel-dev kernel-modules kernel-dbg kernel-image-image kernel-devicetree"
PACKAGESPLITFUNCS="split_kernel_packages split_kernel_module_packages                  package_do_split_locales                 populate_packages"
PACKAGES_DYNAMIC="^linux-raspberrypi-locale-.* ^kernel-module-.* ^kernel-image-.* ^kernel-firmware-.*"
kernel-module-brcmfmac-6.6.63-v8
kernel-module-brcmfmac-bca-6.6.63-v8
kernel-module-brcmfmac-cyw-6.6.63-v8
kernel-module-brcmfmac-wcc-6.6.63-v8
kernel-module-brcmutil-6.6.63-v8
kernel-module-i2c-brcmstb-6.6.63-v8

```
Esto se debe considerar para los distintos kernel-modules que se requieran en la imagen, se debe prestar atencion a la version correspondiente, ya que esta puede variar segun el caso.

## Receta .bb

En esta receta se incluyeron bibliotecas y servicios como mpg123, ALSA, libmicrohttpd, wpa_supplicant, etc, y diferentes kernel-modules especificos con su respectiva version.

```bash

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

```
# Credenciales Wifi

Se deben cambiar las credenciales del Wifi correspondiente, asi como el codigo de pais, en el archivo /meta-robot/recipes-connectivity/wifi-config/files/wpa_supplicant-wlan0.conf


```bash

ctrl_interface=/run/wpa_supplicant
update_config=1
country=COUNTRY_CODE_EXAMPLE:CR

network={
    ssid="Red_Wifi_name"
    psk="password"
    scan_ssid=1
}

```

# Creacion de la imagen

Antes de iniciar, se debe inicializar el entorno build, mediante el comando:

```bash

source oe-init-build-env

```
## Imagen robot-image

1. Primeramente se debe contar con una  imagen minimal, donde no estara el servidor del robot, pero si tendra los servicios minimos deseados como Wifi y salida de audio

```bash

bitbake core-image-minimal

```

2. Para crear la imagen con el meta-robot se necesita asegurarse de incluir el layer en el archivo bblayers, despues ejecutar el comando:

```bash

bitbake robot-image

```

## Comandos de utilidad

1. En caso de cambiar parametros del kernel, se recomienda hacer una limpieza para bitbake, necesaria para eliminar archivos residuales de imagenes anteriores

```bash

bitbake -c cleansstate virtual/kernel

```

2. Menu de configuracion del kernel, para en caso de necesitar activas opciones adicionales de kernel:

```bash

bitbake -c menuconfig virtual/kernel
bitbake -c cleansstate core-image-minimal

```

3.  Verificar el size de la imagen resultante

```bash

ls -lh tmp/deploy/images/raspberrypi4-64/nombre_de_la_imagen.wic.bz2

```

## Verificacion Adicional

Antes de flashear la imagen, es recomendable verificar que se encuentra listo todo lo necesario para las funcionalidades principales, se recomienda ejecutar esta serie de comandos para validad cada seccion.

```bash

ROOTFS="tmp/work/raspberrypi4_64-poky-linux/robot-image/1.0/rootfs"

echo "=== BRCM ===" 
find $ROOTFS/usr/lib/modules -name "*.ko.xz" | grep brcm | sort

echo "=== SND ===" 
find $ROOTFS/usr/lib/modules -name "*.ko.xz" | grep snd | sort

echo "=== FIRMWARE ===" 
ls $ROOTFS/lib/firmware/brcm/ | grep 43455

echo "=== SYSTEMD WANTS ===" 
ls -la $ROOTFS/etc/systemd/system/multi-user.target.wants/

echo "=== WPA CONFIG ===" 
cat $ROOTFS/etc/wpa_supplicant/wpa_supplicant-wlan0.conf

echo "=== ASOUND ===" 
cat $ROOTFS/etc/asound.conf 2>/dev/null || echo "NOT FOUND"

echo "=== MODULES-LOAD ===" 
cat $ROOTFS/usr/lib/modules-load.d/brcmfmac.conf 2>/dev/null
echo "=== MODULES-LOAD WCC ===" 
cat $ROOTFS/usr/lib/modules-load.d/brcmfmac-wcc.conf 2>/dev/null || echo "NO EXISTE"

```

# Flashear imagen a la tarjeta SD

Para flashear la imagen se debe ejecutar una serie de comandos, para esto es necesario tener instalada la herramienta de bmap-tools, la cual se puede instalar con el comando:


```bash

sudo apt update
sudo apt install bmap-tools

```
Adicionalmente, con `df -h`  podemos observar el nombre de la SD card

## Pasos para flashear la imagen

1. Desmontar el sistema de archivos

```bash

sudo umount /route/sd_card/boot
sudo umount /route/sd_card/root

```

2. Ir al directorio donde quedo la imagen resultante

```bash

cd ../yocto/poky-scarthgap/build/tmp/deploy/images/raspberrypi4-64/

```
3. Flashear/copiar la imagen al dispositivo SD

```bash

sudo bmaptool copy robot_image_name.wic.bz2 /dev/sd_card_name

```
4. Finalmente, expulsar de forma segura la tarjeta SD

```bash

sudo eject /dev/sd_card_name

```

Una vez completado este proceso, la tarjeta microSD puede insertarse en la Raspberry Pi 4 para iniciar con la imagen generada. Como consideración adicional, el sistema está configurado para iniciar sesión con el usuario root.
