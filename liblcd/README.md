# lcdlib-st7565p
The lcdlib-st7565p is a C library for microcontroller interface to a write-only serial connection to a ST7565P LCD controller. 

The ST7565P LCD controller does not provide a read mode when connected via a serial interface. As one has to set eight bits at once while writing to the display it is difficult to do graphical operations without read access. This library buffers the display content of specified regions or the whole display in the RAM. For each pixel in these regions one bit of RAM is needed. So for a 128x64 pixel region 8192 bits or 1kbyte of RAM is needed.
While beeing very memory intensive this method allows to do graphic manipulations and drawing without overwriting previous pixels. For the display areas outside of the specified graphic regions only text can be displayed and must be aligned to text lines with a height of eight pixels. 
