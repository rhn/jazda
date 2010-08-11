#!/bin/sh
CC=avr-gcc
CFLAGS='-g -std=gnu99 -Os -W -Wall -mmcu=attiny2313 --whole-program -mcall-prologues -ffreestanding -fpack-struct' #-mno-interrupts'
OBJ2HEX=avr-objcopy 
OBJ2ASM=avr-objdump
AVRDUDE=avrdude

$CC $CFLAGS $1 -o $1.obj
if [ $2 ]; then
   $CC -S $CFLAGS $1 -o $1.asm
fi

$OBJ2HEX -R .eeprom -O ihex $1.obj $1.hex
$OBJ2ASM -C -g -S $1.obj > $1.disasm

$AVRDUDE -c usbasp -p t2313 -U flash:w:$1.hex

