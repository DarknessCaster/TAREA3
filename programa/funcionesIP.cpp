#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ip.h"
#include "funcionesIP.h"
#include "serial.h"
#include "slip.h"

#define MAX_DATA_SIZE 1500
#define MAX_TTL 4

/*  Nombre de la función: encapsularIP
 *  Tipo de función: int
 *  Parámetros: IP &paquete, BYTE TTL, int id, BYTE ip_origen[4], BYTE ip_destino[4].
 *  Descripción de la función: Esta función empaqueta los datos de la estructura IP en el formato adecuado para la transmisión.
 *                             Calcula la longitud de los datos, elimina cualquier salto de línea final, y llena los campos del
 *                             paquete con los valores proporcionados (TTL, id, IP de origen y destino). Además, calcula la suma
 *                             de verificación (FCS) y la incluye en el paquete. Finalmente, retorna la longitud total del paquete.
 */
int encapsularIP(IP &paquete, BYTE TTL, int id, BYTE ip_origen[4], BYTE ip_destino[4]){
    // Almacena longitud de datos
    size_t len;
    len = strlen((const char*)paquete.datos);
    // elimina el salto de linea al final si existe.
    if (len > 0 && paquete.datos[len - 1] == '\n') {
        paquete.datos[len - 1] = '\0';
        len--; // Disminuir la longitud
    }
    paquete.lng_datos[0] = (BYTE)(len & 0xFF); // Byte bajo
    paquete.lng_datos[1] = (BYTE)(len >> 8); // Byte alto
    paquete.TTL = TTL; // Almacena TTL
    paquete.id = id; // Almacena Identificacion
    memcpy(paquete.ip_origen, ip_origen, 4);
    memcpy(paquete.ip_destino, ip_destino, 4);
    // Al  usar indice++ primero asigna y luego suma 1 a la variable indice.
    int indice = 0;
    // Empaqueta Longitud de los datos (16 bits)
    paquete.FRAMES[indice++] = paquete.lng_datos[0]; // Byte bajo
    paquete.FRAMES[indice++] = paquete.lng_datos[1]; // Byte alto

    paquete.FRAMES[indice++] = paquete.TTL & 0xFF; // Empaqueta TTL (8 bits)

    paquete.FRAMES[indice++] = paquete.id & 0xFF; // Empaqueta Identificación (16 bits)

    // Empaqueta IP origen (32 bits)
    memcpy(&paquete.FRAMES[indice], paquete.ip_origen, 4);
    indice += 4;

    // Empaqueta IP destino (32 bits)
    memcpy(&paquete.FRAMES[indice], paquete.ip_destino, 4);
    indice += 4;

    // Calcular la suma de verificación (FCS) solo para la cabecera
    int fcs_ip = fcs(paquete.FRAMES, indice);
    paquete.FCS[0] = (BYTE)(fcs_ip & 0xFF); // Byte alto del FCS
    paquete.FCS[1] = (BYTE)(fcs_ip >> 8); // Byte bajo del FCS

    // Empaquetar suma de verificacion de cabecera
    paquete.FRAMES[indice++] = paquete.FCS[0];
    paquete.FRAMES[indice++] = paquete.FCS[1];

    memcpy(&paquete.FRAMES[indice], paquete.datos, len); // Empaquetar mensaje
    indice += len;
    return indice; // retorna largo del paquete
}

/*  Nombre de la función: desempaquetarIP
 *  Tipo de función: bool
 *  Parámetros: IP &paquete
 *  Descripción de la función: Esta función desempaqueta los datos de la estructura IP.
 *                             Extrae los campos de longitud de datos, TTL, ID, direcciones IP de origen y destino, y FCS del campo FRAMES.
 *                             Luego copia los datos del paquete en el campo correspondiente.
 *                             Retorna true indicando que la operación fue exitosa.
 */
bool desempaquetarIP(IP &paquete){
    size_t indice = 0;
    paquete.lng_datos[0] = paquete.FRAMES[indice++];
    paquete.lng_datos[1] = paquete.FRAMES[indice++];
    short len = (paquete.lng_datos[0] | (paquete.lng_datos[1] << 8));
    paquete.TTL = paquete.FRAMES[indice++];
    paquete.id = paquete.FRAMES[indice++];
    memcpy(paquete.ip_origen, &paquete.FRAMES[indice], 4);
    indice += 4;
    memcpy(paquete.ip_destino, &paquete.FRAMES[indice], 4);
    indice += 4;
    paquete.FCS[0] = paquete.FRAMES[indice++];
    paquete.FCS[1] = paquete.FRAMES[indice++];
    memcpy(paquete.datos, &paquete.FRAMES[indice], len);
    return true;
}

/*  Nombre de la función: convertir_ip
 *  Tipo de función: void
 *  Parámetros: const char* ip_str, BYTE ip[4].
 *  Descripción de la función: Esta función convierte una dirección IP en formato de cadena (por ejemplo, "192.168.1.1")
 *                             a un arreglo de 4 bytes. Si la conversión es exitosa, los octetos de la IP se almacenan
 *                             en el arreglo ip. Si el formato de la cadena no es válido, se muestra un mensaje de error.
 */
void convertir_ip(const char* ip_str, BYTE ip[4]) {
    int octet[4];
    if (sscanf(ip_str, "%d.%d.%d.%d", &octet[0], &octet[1], &octet[2], &octet[3]) == 4) {
        for (int i = 0; i < 4; i++) {
            ip[i] = (BYTE)octet[i];
        }
    } else {
        printf("Formato de IP no válido.\n");
    }
}

/*  Nombre de la función: imprimir_ip
 *  Tipo de función: void
 *  Parámetros: BYTE ip[4].
 *  Descripción de la función: Esta función imprime la dirección IP en el formato 
 *                             "x.x.x.x", donde x es un valor numérico correspondiente
 *                             a cada byte de la dirección IP.
 */
void imprimir_ip(BYTE ip[4]) {
    printf("%c.%c.%c.%c\n", ip[0], ip[1], ip[2], ip[3]);
}

/*  Nombre de la función: fcs
 *  Tipo de función: int
 *  Parámetros: BYTE *arr, int tam.
 *  Descripción de la función: Esta función calcula la suma de verificación (FCS) 
 *                             de un arreglo de bytes. La suma de verificación se 
 *                             obtiene contando el número de bits en 1 en todo el arreglo hasta tam.
 *                             Retorna la suma total de bits en 1.
 */
int fcs(BYTE * arr, int tam){
    int sum_bits = 0;
    for(int i=0; i<tam; i++){
        for(int j=0; j<8; j++){
            sum_bits += (arr[i] >> j) & 0x01;
        } 
    }
    return sum_bits;
}

/*  Nombre de la función: enviarIP
 *  Tipo de función: void
 *  Parámetros: IP paquete, FILE *vport_tx, BYTE ip_origen[4], BYTE ip_destino[4], BYTE TTL
 *  Descripción de la función: Esta función encapsula y envía un paquete IP a través del puerto de transmisión.
 *                             La función solicita al usuario que ingrese un mensaje, lo encapsula en el paquete IP,
 *                             y lo envía utilizando la función writeSlip. La función incrementa un contador de ID para cada paquete enviado.
 */
void enviarIP(IP paquete, FILE *vport_tx, BYTE ip_origen[4], BYTE ip_destino[4], BYTE TTL){
    int contador_id = 1;
    int lng_frame;
    printf("Escriba un mensaje: ");
    fgets((char*)paquete.datos, MAX_DATA_SIZE, stdin); // Almacena mensaje
    lng_frame = encapsularIP(paquete, TTL, contador_id, ip_origen, ip_destino);
    contador_id++;
    writeSlip(paquete.FRAMES, lng_frame, vport_tx);// ENVIAR POR SLIP
}

/*  Nombre de la función: menu_enviar
 *  Tipo de función: int
 *  Parámetros: FILE *vport_tx, BYTE ip_Nodo[4], BYTE ips[6][4].
 *  Descripción de la función: Esta función muestra un menú al usuario para seleccionar el nodo de destino al que desea enviar un mensaje.
 *                             Según la opción seleccionada, la función llama a la función enviarIP con los parámetros correspondientes,
 *                             incluyendo el TTL adecuado para cada nodo de destino.
 */
int menu_enviar(FILE *vport_tx, BYTE ip_Nodo[4], BYTE ips[6][4]){
    int opcion;
    BYTE TTL; 
    IP paquete; // Se inicializa paquete con protocolo IP modificado
    printf("A quien desea enviar el mensaje?\n");
    printf("1. B\n2. C\n3. D\n4. E\n5. A todos (broadcast)\n");
    printf("Ingrese una opcion: ");
    scanf("%d", &opcion);
    getchar();
    switch (opcion) {
        case 1: // NODO B
            TTL = 1;
            enviarIP(paquete, vport_tx, ip_Nodo, ips[1], TTL);
            break;
        case 2: // NODO C
            TTL = 2;
            enviarIP(paquete, vport_tx, ip_Nodo, ips[2], TTL);
            break;
        case 3: // NODO D
            TTL = 3;
            enviarIP(paquete, vport_tx, ip_Nodo, ips[3], TTL);
            break;
        case 4: // NODO E
            TTL = 4;
            enviarIP(paquete, vport_tx, ip_Nodo, ips[4], TTL);
            break;
        case 5: // BROADCAST
            TTL = 4;
            enviarIP(paquete, vport_tx, ip_Nodo, ips[5], TTL);
            break;
        default:
            return 1;
    }
    return 1;
}

/*  Nombre de la función: menu_recibir
 *  Tipo de función: void
 *  Parámetros: FILE *vport_tx, FILE *vport_rx, BYTE ip_Nodo[4], BYTE ips[6][4]
 *  Descripción de la función: Esta función recibe paquetes a través del puerto de recepción, desempaqueta los datos IP,
 *                             verifica el tipo de mensaje (unicast, broadcast) y actúa en consecuencia. Si es necesario,
 *                             retransmite el mensaje al siguiente nodo en la red.
 */
int recibir_mensaje(FILE *vport_tx, FILE *vport_rx, BYTE ip_Nodo[4], BYTE ips[6][4], ruta* tabla_rutas, int num_rutas, char* puerto_rx) {
    IP paquete_rx;
    int len_rx = 0;
    BYTE TTL_rx = 0;
    len_rx = readSlip(paquete_rx.FRAMES, MAX_DATOS_SIZE + 20, vport_rx);
    
    if (len_rx > 0) { 
        desempaquetarIP(paquete_rx);
        short largo = (paquete_rx.lng_datos[0] | (paquete_rx.lng_datos[1] << 8));
        paquete_rx.datos[largo] = '\0';

        // mensaje unicast
        if (memcmp(paquete_rx.ip_destino, ip_Nodo, 4) == 0) {
            printf("Se recibio un mensaje tipo unicast:\n%s\n", paquete_rx.datos);
        }
        // mensaje broadcast
        else if (memcmp(paquete_rx.ip_destino, ips[5], 4) == 0) {
            if (paquete_rx.TTL > 0) {
                paquete_rx.TTL--;
                TTL_rx = MAX_TTL - paquete_rx.TTL;

                // Verificar que no sea el propio nodo que envió el broadcast
                if (memcmp(paquete_rx.ip_origen, ip_Nodo, 4) != 0) {
                    if (paquete_rx.id != 0) {
                        printf("Se recibio un mensaje tipo --broadcast--\n");
                        printf("Mensaje enviado por el nodo %X: %s\n", paquete_rx.ip_origen[0], paquete_rx.datos);
                    }
                    num_rutas = actualizar_rutas(puerto_rx, tabla_rutas, num_rutas, paquete_rx, TTL_rx);
                    encapsularIP(paquete_rx, paquete_rx.TTL, paquete_rx.id, paquete_rx.ip_origen, paquete_rx.ip_destino);
                    writeSlip(paquete_rx.FRAMES, len_rx, vport_tx);
                } else {
                    printf("El mensaje broadcast es propio, se descarta.\n");
                }
            } else {
                printf("El TTL del mensaje ha expirado, se descarta.\n");
            }
        } else {
            paquete_rx.TTL--;
            encapsularIP(paquete_rx, paquete_rx.TTL, paquete_rx.id, paquete_rx.ip_origen, paquete_rx.ip_destino);
            writeSlip(paquete_rx.FRAMES, len_rx, vport_tx);
        }
    }
    return num_rutas;
}


void enviar_broadcast(FILE *vport_tx, FILE *vport_rx, BYTE ip_Nodo[4], BYTE ips[6][4]) {
    struct IP paquete;
    int lng_frame;
    const char* mensaje = "Broadcast para crear tablas de rutas";
    strncpy((char*)paquete.datos, mensaje, MAX_DATA_SIZE);
    lng_frame = encapsularIP(paquete, MAX_TTL, 0, ip_Nodo, ips[5]);
    writeSlip(paquete.FRAMES, lng_frame, vport_tx);
    writeSlip(paquete.FRAMES, lng_frame, vport_rx);
}

int actualizar_rutas(char* puerto_rx, ruta* tabla_rutas, int num_rutas, IP paquete_rx, BYTE TTL_rx) {
    bool actualizado = false;
    for (int i = 0; i < num_rutas; i++) {
        if (memcmp(tabla_rutas[i].ip, paquete_rx.ip_origen, 4) == 0) {
            if (tabla_rutas[i].TTL > TTL_rx) {
                tabla_rutas[i].TTL = TTL_rx;
                snprintf(tabla_rutas[i].puerto, sizeof(tabla_rutas[i].puerto), "%s", puerto_rx); // Usar snprintf para copiar y terminar en nulo
                actualizado = true;
                printf("Tabla de rutas actualizada\n");
                imprimir_rutas(tabla_rutas, num_rutas);
            }
            return num_rutas;
        }
    }
    if (!actualizado && num_rutas < 4) {
        memcpy(tabla_rutas[num_rutas].ip, paquete_rx.ip_origen, 4);
        tabla_rutas[num_rutas].TTL = TTL_rx;
        snprintf(tabla_rutas[num_rutas].puerto, sizeof(tabla_rutas[num_rutas].puerto), "%s", puerto_rx); // Usar snprintf para copiar y terminar en nulo
        num_rutas++;
        actualizado = true;
        printf("Nueva ruta añadida\n");
        imprimir_rutas(tabla_rutas, num_rutas);
    }
    return num_rutas;
}


void imprimir_rutas(ruta* tabla_rutas, int num_rutas) {
    printf("Tabla de rutas:\n");
    for (int i = 0; i < num_rutas; i++) {
        printf("|| Destino: %X.%X.%X.%X || Distancia: %d || Puerto: %s ||\n",
               tabla_rutas[i].ip[0], tabla_rutas[i].ip[1], 
               tabla_rutas[i].ip[2], tabla_rutas[i].ip[3], 
               tabla_rutas[i].TTL, tabla_rutas[i].puerto);
    }
}

void mostrar_menu() {
    printf("Seleccione una opción:\n");
    printf("1. Enviar mensaje broadcast\n");
    printf("2. Enviar mensaje unicast\n");
    printf("3. Mostrar tabla de rutas\n");
    printf("4. Salir\n");
    printf("Opción: ");
}

void ejecutar_opcion(BYTE ips[6][4], int opcion, FILE *vport_b1, FILE *vport_b2, BYTE ip_Nodo[4], ruta tabla_rutas[], int num_rutas) {
    switch (opcion) {
        case 1:
            enviar_broadcast(vport_b1, vport_b2, ip_Nodo, ips);
            break;
        case 2: 
            IP paquete;
            int opc;
            printf("A quien desea enviar el mensaje?\n");
            printf("0. A\n1. B\n2. C\n3. D\n4. E\n5. A todos (broadcast)\n");
            printf("Ingrese una opcion: ");
            scanf("%d", &opc);
            printf("Ingrese el mensaje: ");
            fgets((char*)paquete.datos, MAX_DATA_SIZE, stdin); // Almacena mensaje
            switch (opc) {
                case 0: // NODO A
                    for (int i = 0; i < num_rutas; i++) {
                        if (memcmp(tabla_rutas[i].ip, ips[opcion], 4) == 0) {
                            enviarIP(paquete, vport_b1, ip_Nodo, ips[opcion], tabla_rutas[i].TTL);
                            break;
                        }
                        else{
                            printf("Nodo destino no encontrado\n");
                        }
                    }
                    break;
                case 1: // NODO B
                    for (int i = 0; i < num_rutas; i++) {
                        if (memcmp(tabla_rutas[i].ip, ips[opcion], 4) == 0) {
                            enviarIP(paquete, vport_b1, ip_Nodo, ips[opcion], tabla_rutas[i].TTL);
                            break;
                        }
                        else{
                            printf("Nodo destino no encontrado\n");
                        }
                    }
                    break;
                case 2: // NODO C
                    for (int i = 0; i < num_rutas; i++) {
                        if (memcmp(tabla_rutas[i].ip, ips[opcion], 4) == 0) {
                            enviarIP(paquete, vport_b1, ip_Nodo, ips[opcion], tabla_rutas[i].TTL);
                            break;
                        }
                        else{
                            printf("Nodo destino no encontrado\n");
                        }
                    }
                    break;
                case 3: // NODO D
                    for (int i = 0; i < num_rutas; i++) {
                        if (memcmp(tabla_rutas[i].ip, ips[opcion], 4) == 0) {
                            enviarIP(paquete, vport_b1, ip_Nodo, ips[opcion], tabla_rutas[i].TTL);
                            break;
                        }
                        else{
                            printf("Nodo destino no encontrado\n");
                        }
                    }
                    break;
                case 4: // NODO E
                    for (int i = 0; i < num_rutas; i++) {
                        if (memcmp(tabla_rutas[i].ip, ips[opcion], 4) == 0) {
                            enviarIP(paquete, vport_b1, ip_Nodo, ips[opcion], tabla_rutas[i].TTL);
                            break;
                        }
                        else{
                            printf("Nodo destino no encontrado\n");
                        }
                    }
                    break;
                case 5: // BROADCAST
                    for (int i = 0; i < num_rutas; i++) {
                        if (memcmp(tabla_rutas[i].ip, ips[opcion], 4) == 0) {
                            enviarIP(paquete, vport_b1, ip_Nodo, ips[opcion], tabla_rutas[i].TTL);
                            break;
                        }
                        else{
                            printf("Nodo destino no encontrado\n");
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        case 3:
            imprimir_rutas(tabla_rutas, num_rutas);
            break;
        case 4:
            fclose(vport_b1);
            fclose(vport_b2);
            exit(0);
        default:
            printf("Opción no válida. Intente de nuevo.\n");
    }
}