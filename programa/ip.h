#ifndef IP_H
#define IP_H

#include <stdio.h>
#define BYTE unsigned char
#define MAX_DATOS_SIZE 1500

// Definimos la estructura que representa el protocolo IPv4 modificado
struct IP {
    BYTE flag_fragmento;       // 8 bits para el flag de fragmento
    BYTE offset_fragmento;     // 8 bits para el offset de fragmento
    BYTE TTL;                  // 8 bits para TTL
    BYTE id;                   // 8 bits para identificación
    BYTE lng_datos[2];         // 16 bits para longitud de los datos
    BYTE FCS[2];               // 16 bits para la suma de verificación
    BYTE ip_origen[4];         // 32 bits para la IP de origen
    BYTE ip_destino[4];        // 32 bits para la IP de destino
    BYTE datos[MAX_DATOS_SIZE];          // Arreglo de datos (tamaño variable, máximo 1500 bytes)
    BYTE FRAMES[MAX_DATOS_SIZE + 16];
};

struct ruta {
    BYTE ip[4]; // Destino
    char puerto[20]; // puerto RECEPTOR (buffer suficientemente grande)
    BYTE TTL; // TTL más efectivo

    ruta() {
        TTL = 255;
        puerto[0] = '\0'; // Inicializar puerto como cadena vacía
    }
};
#endif