/**
 * @file datalogger.h
 * @brief COEL E33 DataLogger - Data Logging Library
 * @author Nova Instruments
 */

#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include "modbus.h"

// Configurações do DataLogger
#define DATALOGGER_LOG_DIR "/home/nova"
#define DATALOGGER_MAX_PATH 512
#define DATALOGGER_MAX_LINE 1024

// Estrutura para configuração do datalogger
typedef struct {
    char device_name[32];       // Nome do dispositivo (ex: "NI00002")
    char log_file_path[DATALOGGER_MAX_PATH];  // Caminho completo do arquivo de log
    uint32_t record_counter;    // Contador de registros
    bool initialized;           // Flag de inicialização
    FILE* log_file;            // Handle do arquivo de log
} datalogger_context_t;

// Estrutura para um registro de dados
typedef struct {
    uint32_t record_number;     // Número do registro (R)
    struct tm timestamp;        // Data e hora
    uint16_t temperature;       // TPrincipal (0x200)
    bool door_open;            // PA - Porta Aberta (0x20D)
    bool temp_valid;           // Flag indicando se temperatura é válida
    bool door_valid;           // Flag indicando se status da porta é válido
} datalogger_record_t;

/**
 * @brief Inicializa o sistema de datalogger
 * @param device_name Nome do dispositivo (ex: "NI00002")
 * @return Ponteiro para contexto do datalogger ou NULL em caso de erro
 */
datalogger_context_t* datalogger_init(const char* device_name);

/**
 * @brief Finaliza o sistema de datalogger e libera recursos
 * @param ctx Contexto do datalogger
 */
void datalogger_cleanup(datalogger_context_t* ctx);

/**
 * @brief Registra dados no arquivo de log
 * @param ctx Contexto do datalogger
 * @param modbus_data Dados lidos do Modbus
 * @return true se registro foi bem-sucedido, false caso contrário
 */
bool datalogger_log_data(datalogger_context_t* ctx, const modbus_data_t* modbus_data);

/**
 * @brief Obtém data e hora do RTC do sistema
 * @param tm_info Estrutura para armazenar data/hora
 * @return true se obteve data/hora com sucesso, false caso contrário
 */
bool datalogger_get_rtc_time(struct tm* tm_info);

/**
 * @brief Cria o cabeçalho do arquivo de log
 * @param ctx Contexto do datalogger
 * @return true se cabeçalho foi criado com sucesso, false caso contrário
 */
bool datalogger_create_header(datalogger_context_t* ctx);

/**
 * @brief Converte dados Modbus para registro do datalogger
 * @param modbus_data Dados do Modbus
 * @param record Registro do datalogger a ser preenchido
 * @param record_number Número do registro
 * @return true se conversão foi bem-sucedida, false caso contrário
 */
bool datalogger_convert_modbus_data(const modbus_data_t* modbus_data, 
                                   datalogger_record_t* record, 
                                   uint32_t record_number);

/**
 * @brief Formata e escreve um registro no arquivo de log
 * @param ctx Contexto do datalogger
 * @param record Registro a ser escrito
 * @return true se escrita foi bem-sucedida, false caso contrário
 */
bool datalogger_write_record(datalogger_context_t* ctx, const datalogger_record_t* record);

/**
 * @brief Força a sincronização do arquivo de log com o disco
 * @param ctx Contexto do datalogger
 */
void datalogger_sync(datalogger_context_t* ctx);

/**
 * @brief Obtém informações sobre o arquivo de log atual
 * @param ctx Contexto do datalogger
 * @param file_size Ponteiro para armazenar tamanho do arquivo (pode ser NULL)
 * @param record_count Ponteiro para armazenar número de registros (pode ser NULL)
 * @return true se informações foram obtidas com sucesso, false caso contrário
 */
bool datalogger_get_log_info(datalogger_context_t* ctx, long* file_size, uint32_t* record_count);

/**
 * @brief Imprime estatísticas do datalogger
 * @param ctx Contexto do datalogger
 */
void datalogger_print_stats(datalogger_context_t* ctx);

#endif // DATALOGGER_H
