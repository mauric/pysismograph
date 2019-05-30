# pysismograph
Graficador de vibraciones en python


1) IMERIS_Acel_V2.py: script de python para correr el programa que
grafica los datos del acelerómetro, en Linux. Para lanzarlo: "python3
IMERIS_Acel_V2.py --port /dev/ttyUSB0" por lo general toma el 0, si hay
otros dispositivos conectados hay que ver cual es el puerto que le asigna.

2) mesa_windows_v1.py: script que hace lo mismo pero para pc con
windows. Para lanzarlo: "python3 IMERIS_Acel_V2.py --port COM1" Hay que
ver cual es el puerto COM que utiliza.

3) Aceler_metro_para_mesa.ino: el sketch de arduino que tiene el
acelerómetro.
