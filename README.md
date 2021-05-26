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

    2|10|2|519|510|505|500|493|487|7
    2|11|2|519|510|506|501|495|489|7
    2|12|2|519|511|506|501|495|489|7
    2|13|2|519|511|505|499|493|487|7

where the fields are as follow:

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
    {"version":2,"frame":214,"channel_1":334,"channel_2":511,"channel_3":504,"channel_4":498,"channel_5":492,"channel_6":485,"button_states":7}
    {"version":2,"frame":215,"channel_1":388,"channel_2":511,"channel_3":503,"channel_4":496,"channel_5":489,"channel_6":482,"button_states":15}
    {"version":2,"frame":216,"channel_1":373,"channel_2":511,"channel_3":505,"channel_4":498,"channel_5":491,"channel_6":484,"button_states":15}
    {"version":2,"frame":217,"channel_1":391,"channel_2":511,"channel_3":506,"channel_4":501,"channel_5":495,"channel_6":489,"button_states":15}
