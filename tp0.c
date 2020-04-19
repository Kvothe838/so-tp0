/*
 * main.c
 *
 *  Created on: 28 feb. 2019
 *      Author: utnso
 */

#include "tp0.h"
#define PACKAGESIZE 1024

int main(void)
{
	int conexion;
	char* ip;
	char* puerto;

	t_log* logger;
	t_config* config;

	logger = iniciar_logger();
	config = leer_config();

	puerto = config_get_string_value(config, "PUERTO");
	ip = config_get_string_value(config, "IP");

	if(puerto == NULL || ip == NULL){
		printf("Error leyendo puerto o ip.");
		return(3);
	}

	log_info(logger, "IP: %s | PUERTO: %s\n", ip, puerto);

	conexion = crear_conexion(ip, puerto);

	if(conexion == 0){
		log_info(logger, "No se pudo conectar al servidor.");
		terminar_programa(conexion, logger, config);
		exit(4);
	}

	enviar_mensaje("Hola", conexion);

	char* mensajeRecibido = recibir_mensaje(conexion);

	log_info(logger, "Mensaje recibido: %s", mensajeRecibido);

	free(mensajeRecibido);

	terminar_programa(conexion, logger, config);
}

t_log* iniciar_logger(void)
{
	t_log* logger;

	if((logger = log_create("tp0.log", "client", true, LOG_LEVEL_INFO)) == NULL){
		printf("Error al crear el logger.");
		exit(1);
	}

	return logger;
}

t_config* leer_config(void)
{
	t_config* config;

	if((config = config_create("tp0.config")) == NULL){
		printf("Error leyendo configuración.");
		exit(2);
	}

	return config;
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	if(logger != NULL){
		log_destroy(logger);
	}

	if(config != NULL){
		config_destroy(config);
	}

	if(conexion > 0){
		liberar_conexion(conexion);
	}
}
