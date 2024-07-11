#ifndef FUNCIONESIP_H
#define FUNCIONESIP_H

int encapsularIP(IP &paquete, BYTE TTL, int id, BYTE ip_origen[4], BYTE ip_destino[4]);
void convertir_ip(const char* ip_str, BYTE ip[4]);
void imprimir_ip(BYTE ip[4]);
bool desempaquetarIP(IP &paquete);
int fcs(BYTE * arr, int tam);
void enviarIP(IP paquete, FILE *vport_tx, BYTE ip_origen[4], BYTE ip_destino[4], BYTE TTL);
int menu_enviar(FILE *vport_tx, BYTE ip_Nodo[4], BYTE ips[6][4]);
int recibir_mensaje(FILE *vport_tx, FILE *vport_rx, BYTE ip_Nodo[4], BYTE ips[6][4], ruta* tabla_rutas, int num_rutas, char* puerto_rx);
void enviar_broadcast(FILE *vport_tx, FILE *vport_rx, BYTE ip_Nodo[4], BYTE ips[6][4]);
int actualizar_rutas(char* puerto_rx, ruta* tabla_rutas, int num_rutas, IP paquete_rx, BYTE TTL_rx);
void imprimir_rutas(ruta* tabla_rutas, int num_rutas);
void mostrar_menu();
void ejecutar_opcion(BYTE ips[6][4], int opcion, FILE *vport_b1, FILE *vport_b2, BYTE ip_Nodo[4], ruta tabla_rutas[], int num_rutas);
#endif