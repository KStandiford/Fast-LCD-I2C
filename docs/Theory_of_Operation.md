# Theory of Operation# {#theory}
The key to a fast, efficient LCD display driver using the I2C bus is to realize that the I2C bus is *slow*! This has two immediate implications. First, maximizing the bus throughput will maximize the display update speed. Second, the I2C bus is a lot slower than the LCD display in most cases, so no additional timing considerations are required.

To understand why these things are true, we will first give an overview what has to be done to control the display, followed by a brief overview of the I2C bus. We can then set the goals for this driver software, and discuss the implementation approach. Some implementation limitations from the operating environment must also be considered. Finally, we can explain how the software is implemented, and a bit on how to use its more advanced features.
## Display Control
The LCD display is directly controlled by a display controller, typically the Hitachi HD44780U. The display controller is designed to be interfaced to a parallel data bus with several control lines. In the micro-controller environment, this required a lot of I/O pins, so many people have added an I2C bus interface chip which converts the serial I2C bus to the parallel inputs the display controller needs while only requiring two output pins from the micro-controller.
### Hitachi HD44780U Requirements
The Hitachi HD44780U is the common LCD display controller (**Display Controller**). A data sheet can be found at [Adafruit](https://cdn-shop.adafruit.com/datasheets/HD44780.pdf#page207). The display controller stores 8 bit data values and displays them on the screen. It also accepts 8 bit commands for various purposes such as clearing the display and setting the "input cursor" to a desired location. When interfaced to typical micro-controllers, the display controller is usually used in a "4-bit" mode, where the bytes of data are transmitted as two 4 bit "nibbles". This helps minimize the number of I/O pins needed. The controller also needs three additional interface bits to select between reading and writing, to select between data and command transfers, and an enable pin to clock the data into the chip. There are several important concepts.
- Two nibbles must always be sent. Nothing happens until the second one is received.
- The enable pin functions like a clock signal. Data is sent by first setting the enable pin to 1 and then to 0.
- The control pins for read/write and data/command must be set before enable is set to 1.
- The data pins must be set before the enable pin is set to 0.
- The display controller requires up to 40 micro seconds to process each data byte and most command bytes. (Some commands take longer!) But we note that the controller's interface doesn't do anything until it receives the second nibble, so the transmission can be overlapped with this processing time.
### The I2C Interface
There are a number of I2C interface chips used with the LCD I2C devices. The MCP23008 is an example ([datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/21919e.pdf)). (Note: some of these chips only support the slow I2c speed (100 kHz), so it is important to check the data sheet for the chip in your display before trying to run the I2C bus at higher speed.) The interface monitors the I2C bus. When it sees a transmission with its assigned address, the interface accepts all bytes transmitted on the bus and presents each in turn on its 8 bit parallel outputs. By convention, the 4 upper bits of the output register are wired to 4 of the display controllers data pins. Three of the lower bits of the interface are wired to the display controller enable (or clock) pin, the read/write select pin and the command/data select pin. (We will call the read/write and input/output bits the "mode" bits). The remaining bit controls the display backlight, but this is implemented without using the Hitachi display controller. 
### The Required Output Sequence
So, putting this together, in order to send a byte of data or a command, we must do the following:

1) Pack up the proper mode bits along with the backlight bit, and send it to the interface with enable low. This presets the mode bits for enable going high.

2) Pack up the high nibble of the byte to send with the mode bits and backlight bit, turn on the enable bit and transmit the result. This locks the mode bits into the display controller and presets the data bits for enable going low.

3) Transmit the preceding byte again but with enable low. This clocks data into the display controller.

4) Pack up the low nibble of the byte to send with the mode and backlight bits, turn on the enable bit and transmit the result. This locks the mode bits and presets the data for the second nibble.

5) Finally, transmit the preceding byte again with the enable bit low. This clocks the low order nibble into the display controller and initiates data or command processing.

Thus we see that sending a single character or command to the display controller requires that we send five bytes of data down the I2C bus. (Maybe only four bytes if we are clever. More about this later.) 
## The I2C bus
There are numerous tutorials in great detail about the I2C bus, so we will merely abstract the important points here. 
- The I2C bus is a serial bus. The common clock rates of interest are 100 kHz and 400 kHz, which means it takes 10 uSec or 2.5 uSec respectively to transmit a single bit. 
- Each 8 bit byte transmitted must also include time for an acknowledge bit, so we have a total of 9 bits per character, giving 90 uSec or 22.5 uSec per byte depending on the bus speed. 
- There is no limit on the number of characters which can be sent as a single transmission.
- Each transmission must begin with an address, which adds another 8 bits to the transmission.
- Each transmission has a bus start and stop overhead as well. 
### Conclusions
1) Since we must send a minimum of four bytes to output a single character or command, the I2C bus transmission time is far longer than the display controller processing time (40 uSec) for all but a few commands even at very high bus speeds. For most operations, we do not need any additional delays in the code. (Other published drivers typically have quite a few delays left over from the earlier parallel implementations without the I2C bus.)

2) We want to send the longest possible transmissions to minimize the bus start, stop and address transmission overheads.

3) We want to transmit as little data as possible. In particular, if we know that the mode bits for the current data to be output match the mode bits for the last data sent we do *not* need to pre-send them.
## Software Implementation
### Requirements
1) Eliminate unnecessary delays.

2) Eliminate redundant setting of the mode bits.

3) Make transmissions as long as possible
### Methodology
Requirement 1 is trivial. Just take them out!

Requirement 2 means we need to keep track of the last mode used and compare to the current one needed. If they are not the same, we will output the mode bits to the display before sending any command or data.

Requirement 3 means we need to buffer data into as large a transmission as possible. That leaves a couple of questions...
### When Do We Empty the Buffer?
Without any clues from the user, we have to assume he wishes to see everything right away. All commands and single character writes can still buffer four to five bytes in a single transmission, substantially improving bus utilization and speed. For  string output, we can buffer the entire string. And, of course, should the buffer get full, we must empty it so we don't loose any data.
### How Big Should the Buffer Be?
For a 20 character wide display, the longest string we can output can generate 81 bytes of output (4 per character plus one for the mode bits). So we will try for at least that.
### What About Arduino?
The Arduino I2C interface (Wire) has an internal buffer. Testing revealed it was faster to keep our own buffer and call the wire buffered write command. But we must also pay attention to the buffer length because Wire *discards* data once it's own buffer is full. The buffer length seems to vary. On the Pi Pico it is 128, on the Uno it is 30. So we attempt to discover the length with constants in wire.h, but if we can't find one we default to 30.
## Advanced Topics

### Optimizing a Field Update Use Case
One of the most common use cases is likely to be periodically updating specific fields on the display. This involves sequences of setting the cursor followed by outputting a string.

```
    setCursor(somex,somey);     // position cursor
    writeString("Something");   // display something
    //...possible multiple repeats...
    setCursor(lastx,lasty);     // last cursor position
    writeString("Last");        // update last field
```

This will generate multiple short transmissions. In order to further optimize this use case, an optional parameter enableBuffering has been added to setCursor(), writeChar() and writeString(). If omitted or false, this parameter causes the buffer to be emptied before the request is completed, as expected. If enableBuffering is true, the software will return without emptying the buffer and sending the data to the display. In order to force emptying the buffer, the last output can omit (or set false) the enableBuffering parameter or the method show() can be invoked. 

```
    setCursor(somex,somey,true);    // position cursor, wait to output
    writeString("Something",true);  // display something, wait to output
    setCursor(morex,morey,true);    // next cursor position, wait to output
    writeString("more",true);       // display next, wait to output
    show();                         // output to display now

    //... or ...
    setCursor(lastx,lasty,true);    // last cursor position, wait to output
    writeString("Last");            // update last field, output now
```
The Arduino write and print commands do *not* have the additional parameters. If desired, the writeChar() or writeString() method can be used. This should not be an issue, since people trying to optimize at this level should be using sprintf() instead of print. (IMHO)
## Arduino and Pi Pico?
Yes, it really does compile for either system, based on the ARDUINO environmental variable.
