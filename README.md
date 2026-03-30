# Proyecto1_RobotYocto---Sistemas-Empotrados

Servidor web embebido para el control de un robot aspirador construido con Yocto Project. Expone una API REST sobre HTTP que permite controlar el modo de operación, los motores, la reproducción de audio y consultar el estado general del sistema. La interfaz web se sirve estáticamente desde el mismo proceso.

---

## Arquitectura General

El servidor está construido enteramente en C y corre como un proceso único. Ademas utiliza **libmicrohttpd** con un hilo por conexión. La biblioteca de audio corre su propio hilo de reproducción y se comunica con el hilo principal mediante mutex y variables de condición.

---

## Dependencias del Sistema

| Biblioteca          | Paquete (Debian/Ubuntu)          | Descripción                                  |
|---------------------|----------------------------------|----------------------------------------------|
| libmicrohttpd       | `libmicrohttpd-dev`              | Servidor HTTP embebido                       |
| libmpg123           | `libmpg123-dev`                  | Decodificación de archivos MP3               |
| libasound2 (ALSA)   | `libasound2-dev`                 | Salida de audio PCM                          |
| mpg123 (binario)    | `mpg123`                         | Reproducción de notificaciones de audio      |

La biblioteca del audio utiliza **mpg123** para decodificación MP3 y **ALSA** para la salida de audio PCM. Internamente mantiene un hilo de reproducción dedicado que recibe comandos mediante una variable de condición. Las notificaciones de sistema se reproducen mediante un proceso hijo independiente del hilo principal de reproducción, de forma que no interrumpen la música en curso.

---

## Estructura del Proyecto

> El directorio `audio/` debe existir en el mismo directorio desde donde se ejecuta el binario. Los archivos de notificación son opcionales; si no existen, se omiten sin error.

---

## Configuración Previa

### Directorio de audio

El servidor busca archivos `.mp3` en `./audio/` de forma predeterminada:

```bash
mkdir -p audio
cp /ruta/track.mp3 audio/
```

Los archivos de notificación deben nombrarse como:

```
audio/notify_startup.mp3
audio/notify_autonomous.mp3
audio/notify_obstacle.mp3
audio/notify_manual.mp3
```

### Usuario y credenciales

Las credenciales de acceso se configuran en `auth.c`. Para agregar mas usuarios se debe incluir su username y su password pero encriptada mediante SHA256

---

## Compilación

```bash
make clean && make
```
---

## Ejecución

```bash
make run
```

### Acceso a la interfaz web

Una vez levantado el servidor, se puede acceder mediante:

```
http://localhost:8080          # Desde la misma máquina
http://<IP-del-robot>:8080     # Desde la red local
```
---
