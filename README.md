# Solr

The **Solr digital wrist watch** is a clock that won’t work without a battery but it will neither work without sun. Even more: the vintage display is really cool but it’s hard to read outdoors. A complete nonsense.

![Solr Watch](/images/20170614_135212s.jpg)

Check my **blog post** to know more about it: [Solr: digital wrist watch](http://tinkerman.cat/solr-digital-wrist-watch/)


## Hardware

I strongly recommend to find some provider that can assembly at least the HMC5883L, it's a really hard to chip to solder manually. The rest of the components are quite easy to do.

### BOM as exported from Eagle

|Part|Value|Package|Buy|
|---|---|---|---|
|BATT||CR2032-SMD|.|
|C1|100nf|C0805|.|
|C2|4.7uf|AVX-A|.|
|C3|220nf|C0805|.|
|C4|100nf|C0805|.|
|CHECK||BTN_CK_KSS|[Aliexpress](http://s.click.aliexpress.com/e/MrBaM7q)|
|IC1|ATMEGA328P-AU-TQFP32|TQFP32-0.8-7X7MM|[Aliexpress](http://s.click.aliexpress.com/e/Y3FMzZV)|
|IC2|HMC5883L|PLCC16-0.5-3X3MM|[Aliexpress](http://s.click.aliexpress.com/e/bIAm2zB) / [Ebay](https://rover.ebay.com/rover/1/711-53200-19255-0/1?icep_id=114&ipn=icep&toolid=20004&campid=5338044841&mpre=http%3A%2F%2Fwww.ebay.com%2Fitm%2F1x-NEW-HONEYWELL-HMC5883L-HMC-5883L-LCC16-LCC-16-IC-CHIPS-%2F321121197770)|
|LED1|HP QDSP-6064||[Ebay](https://rover.ebay.com/rover/1/1185-53479-19255-0/1?icep_id=114&ipn=icep&toolid=20004&campid=5338044841&mpre=http%3A%2F%2Fwww.ebay.es%2Fitm%2FRetro-4-digit-BUBBLE-VINTAGE-rare-red-LED-Display-QDSP6064-NOS-%2F152160249337)|
|R1|82R|R0805|.|
|R2|82R|R0805|.|
|R3|82R|R0805|.|
|R4|82R|R0805|.|
|R5|2.2K|R0805|.|
|R6|2.2K|R0805|.|
|R7|4.7K|R0805|.|


## Firmware

Check the blog post above to connect an AVR programmer to the board.

The project is ready to be build using [PlatformIO](http://www.platformio.org).
Please refer to their web page for instructions on how to install the builder.
Once installed:

```bash
> cd code
> pio run -e dev
> avrdude -v -p atmega328p -C avrdude.conf -c usbasp -Pusb -U flash:w:.pioenvs/dev/firmware.hex:i

```

Library dependencies are automatically managed via PlatformIO Library Manager.

Using PlatformIO to flash the board will probably result in an error since, by default, PlatformIO forces the -D flag on the compiler that skips the erasing process prior to flash the new project, then the verification will likely fail because out-of-boundaries memory positions will contain garbage data. That's why I'm compiling the code with PlatfomIO and flashing it directly using avrdude.
