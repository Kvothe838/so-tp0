#include "utils.h"

void* serializar_paquete(t_paquete* paquete, int *bytes)
{
	*bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = malloc(*bytes);
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
	offset += paquete->buffer->size;

	return a_enviar;
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
		socket_cliente = 0;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	int tamanioString = strlen(mensaje);
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->codigo_operacion = MENSAJE;
	paquete->buffer->size = tamanioString + 1;// + sizeof(int);
	paquete->buffer->stream = malloc(paquete->buffer->size);//malloc(paquete->buffer->size);

	memcpy(paquete->buffer->stream, mensaje, tamanioString + 1);

	int bytes = 0;
	void* mensajeSerializado = serializar_paquete(paquete, &bytes);

	if(send(socket_cliente, mensajeSerializado, bytes, 0) == -1)
		printf("Error enviando mensaje.");

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	free(mensajeSerializado);
}

char* recibir_mensaje(int socket_cliente)
{
	int codigo_operacion;
	int size = 0;
	char* buffer;
	recv(socket_cliente, &codigo_operacion, sizeof(int), 0);

	switch(codigo_operacion) {
		case MENSAJE:

			recv(socket_cliente, &size, sizeof(int), 0);
			buffer = malloc(size);
			recv(socket_cliente, buffer, size, 0);

			break;
		default:
			printf("Error: código no válido.\n");
			break;
	}
	return buffer;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
