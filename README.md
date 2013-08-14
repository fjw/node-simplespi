node-simplespi
==============

This is a simple and easy to use **SPI** module for **node.js**.

I wrote this to have an easy way to control a **Adafruit PWM LED driver** with my **Raspberry Pi** ( [node-leddriver](https://github.com/fjw/node-leddriver) ), but it **should also work with different clients and hosts**.

### Installation

```sh
npm install simplespi
```

### Usage

```js
var simplespi = require( "simplespi" );

var response = simplespi.send( "AA00FF", "/dev/spidev0.1" );
```

The `send` method accepts a hexcode string as parameter and sends it to the SPI device defined in the optional second parameter.
If the second parameter is undefined it uses `/dev/spidev0.0`.

The value returned is the response of the device in hexcode string form.

### Testing on the Pi

First run the following on your Raspberry Pi (with `root` privileges):
```js
console.log(simplespi.send( "AA00FF" ));
```
It should return:
```
000000
```
Then shortcut the `MISO` and `MOSI` pins of your pi and run the same command again.
It should then return:
```
AA00FF
```
If you got this your SPI device is working.

### Enabling SPI on the Pi

If it's still not working, check your `/etc/modprobe.d/raspi-blacklist.conf` and comment out the entry:
```
blacklist spi-bcm2708
```
by changing it to:
```
#blacklist spi-bcm2708
```
then reboot your pi and type `ls /dev`.
If `spidev0.0` is in the list you're fine and you can repeat the test.