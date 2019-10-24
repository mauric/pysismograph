IMERIS MESVIB DATA SENDER
---------------------------



TODO

- crear inputs fields y concatenar todo en una variable mensaje
- mostrar ese mensaje en un caja o campo o label par verificar por el usuario
- mostrar lo que se recibe para verificar por el usuario y verificar por software que este bien enviado
-

Journal


Instalar Qt creator en la pc desktop


Luego instalar un toolchain en el Host es decir los paquetes del debugger GDB-MULTIARCH, el toolchain para raspberry pi, qmake para raspberry pi3 y un raspbian sysysroot
En la raspberry pi que se pueda conectar con una ssh y librerias qt instaladas en la raspberry pi (supongo que deberán ser usadas por el ejecutable luego)

    Install development libraries – [Pi]
    Prepare target folder – [Pi]
    Create working folder and set a toolchain – [Co]
    Create and configure a sysroot – [Co]
    Download Qt – [Co]
    Configure Qt for cross compilation – [Co]
    Compile, install and deploy Qt – [Co]
    Setup Qt Creator for Raspberry Pi cross compilation – [Co]


Estos son los pasos basicos a seguir según
https://mechatronicsblog.com/cross-compile-and-deploy-qt-5-12-for-raspberry-pi/ 


COMPILACION CRUZADA NECESITA UN SYSROOT
un sysroot es un directorio que contiene lo esencial de un sistema operativo. Es una estructura con lo basico de un sistema para correr. En linux es esta represtando por el tipico directorio "/" donde cuelga todo el sistema. 
Puede ser usado para realizar compilacion cruzada, donde se pueden dejar dentro
directorio comunes en linux como /usr/lib, /usr/include, etc.



SEGUN ESTE TUTORIAL

https://medium.com/@amirmann/how-to-cross-compile-qt-for-raspberry-pi-3-on-linux-ubuntu-for-beginners-75acf2a078c

Es necesario tener claramente un linux instlada en la computadora host
y en la target una distribucion linux tambie'n 
