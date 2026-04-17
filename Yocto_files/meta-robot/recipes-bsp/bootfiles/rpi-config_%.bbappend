ENABLE_UART = "1"
RPI_EXTRA_CONFIG = " \
dtparam=audio=on \n \
audio_pwm_mode=2 \n \
dtparam=krnbt=off \n \
"
MACHINE_FEATURES:append = " wifi"
