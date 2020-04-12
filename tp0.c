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

	log_info(logger, "Empieza.");

	config = leer_config();

	puerto = config_get_string_value(config, "PUERTO");
	ip = config_get_string_value(config, "IP");

	conexion = crear_conexion(ip, puerto);

	if(conexion == 0){
		log_info(logger, "No se pudo conectar al servidor.");
		terminar_programa_ante_desconexion_servidor(logger, config);
		return 1;
	}

	log_info(logger, "Se envía un mensaje.");
	enviar_mensaje("Hola", conexion);

	char* mensajeRecibido = recibir_mensaje(conexion);

	log_info(logger, mensajeRecibido);

	free(mensajeRecibido);

	log_info(logger, "Termina");

	terminar_programa(conexion, logger, config);
}

t_log* iniciar_logger(void)
{
	return log_create("tp0.log", "client", true, LOG_LEVEL_INFO);
}

t_config* leer_config(void)
{
	return config_create("tp0.config");
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
}

void terminar_programa_ante_desconexion_servidor(t_log* logger, t_config* config){
	log_destroy(logger);
	config_destroy(config);
}
