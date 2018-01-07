# eegreader
data reader for the OpenEEG firmware v2 protocol

this is a work in progress utility to extract raw data from
olimex eeg-smt device but can be adapted to work with other
OpenEEG firmware v2 compliant devices.

this is written for the OpenBSD operating system but it has
been tested on Linux and OSX succesfully.

1- make && make install
2- doas eegreader -l /dev/cuaUO

the output will consists of | separated fields like:

    ea|2|519|510|505|500|493|487|7
    eb|2|519|510|506|501|495|489|7
    ec|2|519|511|506|501|495|489|7
    ed|2|519|511|505|499|493|487|7

where the fields are as follow:

    frame
    version
    channel 1
    channel 2
    channel 3
    channel 4
    channel 5
    channel 6
    button states

Each channel value is a 10-bit integer.