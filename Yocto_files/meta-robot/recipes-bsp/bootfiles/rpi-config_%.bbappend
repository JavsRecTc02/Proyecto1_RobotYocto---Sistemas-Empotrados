ENABLE_UART = "1"
RPI_EXTRA_CONFIG = " \
dtparam=audio=on\n\
audio_pwm_mode=2\n\
dtparam=krnbt=off\n\
hdmi_ignore_edid_audio=1\n\
hdmi_force_hotplug=1\n\
hdmi_group=2\n\
hdmi_mode=82\n\
"
MACHINE_FEATURES:append = " wifi"
