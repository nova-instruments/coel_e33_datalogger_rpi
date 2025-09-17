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
#include <sqlite3.h>
#include "modbus.h"

// Configurações do DataLogger
#define DATALOGGER_LOG_DIR "/home/nova"
#define DATALOGGER_MAX_PATH 512
#define DATALOGGER_MAX_LINE 1024

// Estrutura para configuração do datalogger
typedef struct {
    char device_name[32];       // Nome do dispositivo (ex: "NI00002")
    char log_file_path[DATALOGGER_MAX_PATH];  // Caminho completo do arquivo de log TXT
    char db_file_path[DATALOGGER_MAX_PATH];   // Caminho completo do arquivo de banco SQLite
    uint32_t record_counter;    // Contador de registros
    bool initialized;           // Flag de inicialização
    FILE* log_file;            // Handle do arquivo de log TXT
    sqlite3* db;               // Handle do banco de dados SQLite
} datalogger_context_t;

// Estrutura para um registro de dados (formato TXT)
typedef struct {
    uint32_t record_number;     // Número do registro (R)
    struct tm timestamp;        // Data e hora
    uint16_t temperature;       // TPrincipal (0x200)
    bool door_open;            // PA - Porta Aberta (0x20D)
    bool temp_valid;           // Flag indicando se temperatura é válida
    bool door_valid;           // Flag indicando se status da porta é válido
} datalogger_record_t;

// Estrutura para registro no banco SQLite (sem coluna Degelo)
typedef struct {
    int IndexID;               // Chave primária (auto-incremento)
    long long CollectTime;     // Timestamp em milissegundos
    float Tprincipal;          // Temperatura principal em °C (2 casas decimais)
    int Porta;                 // Status da porta (0=fechada, 1=aberta)
} datalogger_db_record_t;

// Estrutura para informações do banco (tabela DBInfo)
typedef struct {
    int version;               // Versão do banco
    int MaxID;                 // Maior ID registrado
    int MinID;                 // Menor ID registrado
    long long StartTime;       // Timestamp de início
    long long EndTime;         // Timestamp de fim
    int Value0;                // Valor reservado 0
    int Value1;                // Valor reservado 1
    int Value2;                // Valor reservado 2
    int Value3;                // Valor reservado 3
    int Value4;                // Valor reservado 4
} datalogger_db_info_t;

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

/**
 * @brief Inicializa o banco de dados SQLite
 * @param ctx Contexto do datalogger
 * @return true se inicialização foi bem-sucedida, false caso contrário
 */
bool datalogger_init_database(datalogger_context_t* ctx);

/**
 * @brief Cria as tabelas do banco de dados
 * @param ctx Contexto do datalogger
 * @return true se criação foi bem-sucedida, false caso contrário
 */
bool datalogger_create_tables(datalogger_context_t* ctx);

/**
 * @brief Converte registro TXT para registro do banco
 * @param txt_record Registro no formato TXT
 * @param db_record Registro no formato do banco (a ser preenchido)
 * @return true se conversão foi bem-sucedida, false caso contrário
 */
bool datalogger_convert_to_db_record(const datalogger_record_t* txt_record,
                                    datalogger_db_record_t* db_record);

/**
 * @brief Insere registro no banco de dados
 * @param ctx Contexto do datalogger
 * @param db_record Registro a ser inserido
 * @return true se inserção foi bem-sucedida, false caso contrário
 */
bool datalogger_insert_db_record(datalogger_context_t* ctx,
                                const datalogger_db_record_t* db_record);

/**
 * @brief Atualiza informações do banco (tabela DBInfo)
 * @param ctx Contexto do datalogger
 * @return true se atualização foi bem-sucedida, false caso contrário
 */
bool datalogger_update_db_info(datalogger_context_t* ctx);

/**
 * @brief Finaliza o banco de dados SQLite
 * @param ctx Contexto do datalogger
 */
void datalogger_cleanup_database(datalogger_context_t* ctx);

#endif // DATALOGGER_H
