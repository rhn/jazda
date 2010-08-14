#!/bin/sh
CC=avr-gcc
CFLAGS='-g -std=gnu99 -Os -W -Wall --whole-program -mcall-prologues -ffreestanding -fpack-struct' #-mno-interrupts'
OBJ2HEX=avr-objcopy 
OBJ2ASM=avr-objdump
AVRDUDE=avrdude

# parts: t2313 m8
if [ $2 = 't2313' ]; then
  CFLAGS=$CFLAGS' -mmcu=attiny2313 -DATTINY2313'
  echo $CFLAGS
fi
if [ $2 = 'm8' ]; then
  CFLAGS=$CFLAGS' -mmcu=atmega8 -DATMEGA8'
fi

$CC $CFLAGS $3 -o $3.obj
if [ $4 ]; then
   $CC -S $CFLAGS $3 -o $3.asm
fi

$OBJ2HEX -R .eeprom -O ihex $3.obj $3.hex
$OBJ2ASM -C -g -S $3.obj > $3.disasm

$AVRDUDE -c $1 -p $2 -U flash:w:$3.hex

