#/bin/bash
/home/xose/.platformio/packages/tool-avrdude/avrdude -v -p atmega328p -C /home/xose/.platformio/packages/tool-avrdude/avrdude.conf -c usbasp -Pusb \
    -U lfuse:w:0xE2:m \
    -U hfuse:w:0xD9:m \
    -U efuse:w:0xFF:m
