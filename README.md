# eegreader
data reader for the OpenEEG firmware v2 protocol

this is a work in progress utility to extract raw data from
olimex eeg-smt device but can be adapted to work with other
OpenEEG firmware v2 compliant devices.

this is written for the OpenBSD operating system but it has
been tested on Linux and OSX succesfully.

    $ make && make install
    $ doas eegreader -l /dev/cuaUO

the output will consists of | separated fields like:

    1622034547374070|2|10|519|510|505|500|493|487|7
    1622034547374071|2|11|519|510|506|501|495|489|7
    1622034547387645|2|12|519|511|506|501|495|489|7
    1622034547387646|2|13|519|511|505|499|493|487|7

where the fields are as follow:

    timestamp (microseconds)
    version
    frame
    channel 1
    channel 2
    channel 3
    channel 4
    channel 5
    channel 6
    button states

Each channel value is a 10-bit integer.

Alternatively, the `-j` option allows a json-formatted output:

    $ doas eegreader -l /dev/cuaUO -j
    {"timestamp:": 1622034547374070,"version":2,"frame":24,"channel_1":369,"channel_2":511,"channel_3":504,"channel_4":497,"channel_5":490,"channel_6":483,"button_states":7}
    {"timestamp:": 1622034547374071,"version":2,"frame":25,"channel_1":373,"channel_2":511,"channel_3":505,"channel_4":499,"channel_5":492,"channel_6":486,"button_states":7}
    {"timestamp:": 1622034547387645,"version":2,"frame":26,"channel_1":405,"channel_2":511,"channel_3":506,"channel_4":501,"channel_5":495,"channel_6":489,"button_states":7}
    {"timestamp:": 1622034547387646,"version":2,"frame":27,"channel_1":417,"channel_2":511,"channel_3":505,"channel_4":499,"channel_5":493,"channel_6":487,"button_states":15}
