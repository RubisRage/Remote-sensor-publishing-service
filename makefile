# Uncomment lines below if you have problems with $PATH
#SHELL := /bin/bash
#PATH := /usr/local/bin:$(PATH)

all:
				platformio run 

compiledb:
				platformio run --target compiledb

upload:
				platformio run --target upload -e collector
				platformio run --target upload -e sensor --upload-port /dev/ttyACM1
				platformio run --target upload -e sensor --upload-port /dev/ttyACM2

clean:
				platformio run --target clean

program:
				platformio run --target program

update:
				platformio update
