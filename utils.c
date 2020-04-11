/*
 * conexiones.c
 *
 *  Created on: 2 mar. 2019
 *      Author: utnso
 */

#include "utils.h"

void* serializar_paquete(t_paquete* paquete, int *bytes)
{
	int int_size = sizeof(int);
	*bytes = paquete->buffer->size + 2 * int_size;
	void* a_enviar = malloc(*bytes);
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), int_size);
	offset += int_size;
	memcpy(a_enviar + offset, &(paquete->buffer->size), int_size);
	offset += int_size;
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	return a_enviar;
}

char* deserializar_paquete(int socket_cliente) {
	void * buffer;
	int size;

	recv(socket_cliente, &size, sizeof(int), 0);
	buffer = malloc(size);
	recv(socket_cliente, buffer, size, 0);

    return (char*)buffer;
}

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1){
		printf("\nError conectando socket\n");
		socket_cliente = 0;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	int size_of_stream = sizeof(mensaje) + 1;
	int size_of_buffer = sizeof(int) + size_of_stream;
	t_paquete* paquete = malloc(sizeof(int) + size_of_buffer);
	t_buffer* buffer = malloc(size_of_buffer);

	buffer->size = size_of_stream;
	buffer->stream = malloc(buffer->size);
	paquete->codigo_operacion = MENSAJE;

	memcpy(buffer->stream, mensaje, size_of_stream);

	paquete->buffer = buffer;

	int bytes = 0;
	void* mensaje_serializado = serializar_paquete(paquete, &bytes);

	if(send(socket_cliente, mensaje_serializado, bytes, 0) == -1)
		printf("Error enviando mensaje.");

	free(mensaje_serializado);
}

char* recibir_mensaje(int socket_cliente)
{
	int codigo_operacion;
	char* mensaje = "";
	int resultado_recv = recv(socket_cliente, &codigo_operacion, sizeof(int), 0);

	switch(resultado_recv){
		case 0:
			printf("Desconexión.\n");
			return mensaje;
		case -1:
			printf("Error al recibir codigo_operacion.\n");
			return mensaje;
	}

	switch(codigo_operacion) {
	    case MENSAJE:
	        mensaje = deserializar_paquete(socket_cliente);
	        break;
	    default:
	    	printf("Alto fail, codigo_operacion diferente.\n");
	    	break;
	}

	return mensaje;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
