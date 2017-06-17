#/bin/bash
pio run -e dev
/home/xose/.platformio/packages/tool-avrdude/avrdude -v -p atmega328p -C /home/xose/.platformio/packages/tool-avrdude/avrdude.conf -c usbasp -Pusb -U flash:w:.pioenvs/dev/firmware.hex:i
