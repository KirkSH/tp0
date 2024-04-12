#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion_fd = 1;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"
	log_info(logger, "Hola! Soy un Log");

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config_cliente(logger);

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	valor = config_get_string_value(config, "CLAVE");

	// Loggeamos el valor de config
	log_info(logger, valor);

	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/
	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion_fd = crear_conexion(ip, puerto);

// ======================================================
// aca ESTOY
// ======================================================
	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje(valor, conexion_fd);

	// Armamos y enviamos el paquete
	paquete(conexion_fd);

	terminar_programa(conexion_fd, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente

	return 0;
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger = log_create("tp0.log", "prueba de logs tp0", true, LOG_LEVEL_INFO);

	if (nuevo_logger == NULL)
	{
		printf("no se pudo instanciar el logger\n");
		exit(3);

	} else
	{
		printf("logger instanciado\n");
	}

	return nuevo_logger;
}

t_config* iniciar_config_cliente(t_log* logger)
{
	t_config* nuevo_config = config_create("/home/utnso/recursosSO/tp0/client/cliente.config");
	// Le puse hardcodeada la ruta del config según mi VM ubuntu server.

	if (nuevo_config == NULL)
	{
		log_error(logger, "archivo  \"cliente.config\" no encontrado");
		log_info(logger, "no se pudo instanciar la config del cliente");
		exit(3);

	} else
	{
		log_trace(logger, "config del cliente instanciada");
	}

	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	log_trace(logger, "comienza lectura de consola");

	char* leido;

	// La primera te la dejo de yapa
	leido = readline("> ");

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
	while (!string_is_empty(leido))
	{
		log_info(logger, leido);
		free(leido);

		leido = readline("> ");
	}

	// ¡No te olvides de liberar las lineas antes de regresar!
	free(leido);

	log_trace(logger, "finaliza lectura de consola");
}

void paquete(int socket_conexion_fd)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete = crear_paquete();

	// Leemos y esta vez agregamos las lineas al paquete
	leido = readline("> ");
	while (!string_is_empty(leido))
	{
		int tamanio_leido = strlen(leido) + 1;
		agregar_a_paquete(paquete, leido, tamanio_leido);

		free(leido);

		leido = readline("> ");
	}

	enviar_paquete(paquete, socket_conexion_fd);

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	free(leido);
	eliminar_paquete(paquete);
}

void terminar_programa(int socket_conexion_fd, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	int err = close(socket_conexion_fd);
	if(err != 0)
	{
		log_error(logger, "error en funcion close()\n");
		exit(3);
	}
	log_destroy(logger);
	config_destroy(config);
}
