# Proyecto1_RobotYocto---Sistemas-Empotrados
Repositorio dedicado al proyecto 1 del curso de Sistemas Empotrados IS2026

### Layer necesarios para la imagen

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

### Comandos Utiles de bitbake

1. Se puede crear la imagen minimal, donde no estara el servidor del robot, pero si tendra los servicios minimos deseados

```bash

bitbake core-image-minimal

```

2. Para crear la imagen con el meta-robot se necesita ejecutar el comando:

```bash

bitbake robot-image

```

3. Limpieza para bitbake, necesaria para eliminar archivos residuales de imagenes anteriores

```bash

bitbake -c cleansstate virtual/kernel

```

4. Menu de configuracion del kernel:

```bash

bitbake -c menuconfig virtual/kernel
bitbake -c cleansstate core-image-minimal

```

5.  Verificar el size de la imagen creada

```bash

ls -lh tmp/deploy/images/raspberrypi4-64/nombre_de_la_imagen.wic.bz2

```
