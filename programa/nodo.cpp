#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "serial.h"
#include "slip.h"
#include "ip.h"
#include "funcionesIP.h"

#define MAX_DATA_SIZE 1500
BYTE ips[6][4] = {
    {0xA, 0xA, 0xA, 0xA},  // ip_A
    {0xB, 0xB, 0xB, 0xB},  // ip_B
    {0xC, 0xC, 0xC, 0xC},  // ip_C
    {0xD, 0xD, 0xD, 0xD},  // ip_D
    {0xE, 0xE, 0xE, 0xE},  // ip_E
    {0xF, 0xF, 0xF, 0xF}   // ip_Broadcast
};

int main(int nargs, char* arg_arr[]){
    if(nargs == 4){
        BYTE ip_Nodo[4];
        ruta tabla_rutas[4];
        int num_rutas = 0;
        // Obtener ip del nodo y sus respectivos puertos. 
        char* ip_nodo = arg_arr[1]; 
        char* puerto_b1 = arg_arr[2];
        char* puerto_b2 = arg_arr[3]; 
        // Abrir puertos bidireccionales
        FILE *vport_b1 = fopen(puerto_b1, "r+"); 
        FILE *vport_b2 = fopen(puerto_b2, "r+"); 

        convertir_ip(ip_nodo, ip_Nodo);
        // Bucle infinito para enviar y recibir mensajes
        while (1) {
            // Automatizacion de tablas de rutas
            enviar_broadcast(vport_b1, vport_b2, ip_Nodo, ips); //enviar broadcast
            //recibir broadcast
            num_rutas = recibir_mensaje(vport_b1, vport_b2, ip_Nodo, ips, tabla_rutas, num_rutas, puerto_b2);
            num_rutas = recibir_mensaje(vport_b2, vport_b1, ip_Nodo, ips, tabla_rutas, num_rutas, puerto_b1);
            sleep(5); // envia cada 5 segundos

            // Verificar si hay entrada del usuario para mostrar el menú
            if (kbhit()) {
                int opcion;
                mostrar_menu();
                scanf("%d", &opcion);
                ejecutar_opcion(opcion, vport_b1, vport_b2, ip_Nodo, tabla_rutas, &num_rutas);
            }
        }
        fclose(vport_b1);
        fclose(vport_b2);
    } 
    else{
        printf("--------------------MANUAL DE USUARIO--------------------\n");
        printf("---------------------------------------------------------\n");
        printf("Para ejecutar el programa debe inicializar el nodo con su\nrespectiva letra usando el comando:\n\n");
        printf("\t make run_nodo_(Letra del nodo)\n\n");
        printf("Ejemplos:\n\n");
        printf("\t Nodo A:\t make run_nodo_A\n");
        printf("\t Nodo B:\t make run_nodo_B\n");
        printf("\t Nodo C:\t make run_nodo_C\n");
        printf("\t Nodo D:\t make run_nodo_D\n");
        printf("\t Nodo E:\t make run_nodo_E\n\n");
        printf("Descripción del Programa:\n");
        printf("-------------------------\n");
        printf("Este programa simula una red de nodos interconectados en un anillo. Cada nodo puede enviar o recibir\n");
        printf("mensajes a otros nodos a través de un protocolo IP modificado. Los mensajes pueden ser unicast (a un\nsolo nodo)");
        printf("o broadcast (a todos los nodos).\n\n");
    }
    return 0;
}
