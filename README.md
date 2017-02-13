# eegreader
data reader for the OpenEEG firmware v2 protocol

this is a work in progress utility to extract raw data from
olimex eeg-smt device but can be adapted to work with other
OpenEEG firmware v2 compliant devices.

this is written for the OpenBSD operating system.

1- make && make install
2- doas eegreader -l /dev/cuaUO

the output will consists of | separated fields like:

    e0|2|2,5|1,255|1,255|1,249|1,244|1,237|1,230|7
    e1|2|0,232|1,255|1,255|1,249|1,243|1,237|1,230|7
    e2|2|1,80|1,255|1,255|1,248|1,242|1,235|1,228|7
    e3|2|2,181|1,255|1,255|1,250|1,245|1,240|1,234|7

where the fields are as follow:

    frame
    version
    channel 1 low, channel 1 high
    channel 2 low, channel 2 high
    channel 3 low, channel 3 high
    channel 4 low, channel 4 high
    channel 5 low, channel 5 high
    channel 6 low, channel 6 high
    button states
