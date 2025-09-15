/**
 * @file modbus.h
 * @brief COEL E33 DataLogger - Modbus RTU Library
 * @author Nova Instruments
 */

#ifndef MODBUS_H
#define MODBUS_H

#include <stdint.h>
#include <stdbool.h>

// Forward declaration para evitar dependência circular
typedef struct modbus_t modbus_t;

// Configurações Modbus
#define MODBUS_DEVICE     "/dev/serial0"
#define MODBUS_BAUD_RATE  9600
#define MODBUS_PARITY     'N'
#define MODBUS_DATA_BITS  8
#define MODBUS_STOP_BITS  1
#define MODBUS_SLAVE_ID   1

// Endereços Modbus
#define MODBUS_ADDR_0x200 0x200
#define MODBUS_ADDR_0x20D 0x20D

// Timeouts (em microssegundos)
#define MODBUS_RESPONSE_TIMEOUT_US 500000  // 500ms
#define MODBUS_BYTE_TIMEOUT_US     200000  // 200ms

// Estrutura para dados lidos
typedef struct {
    uint16_t addr_0x200;    // Valor do registrador 0x200
    uint16_t addr_0x20d;    // Valor do registrador 0x20D
    bool addr_0x20d_binary; // Interpretação binária de 0x20D (0 ou 1)
    bool valid_0x200;       // Flag indicando se leitura de 0x200 foi bem-sucedida
    bool valid_0x20d;       // Flag indicando se leitura de 0x20D foi bem-sucedida
} modbus_data_t;

// Handle opaco para contexto Modbus
typedef struct modbus_context_s modbus_context_t;

/**
 * @brief Inicializa conexão Modbus
 * @return Ponteiro para contexto Modbus ou NULL em caso de erro
 */
modbus_context_t* modbus_init(void);

/**
 * @brief Finaliza conexão Modbus e libera recursos
 * @param ctx Contexto Modbus
 */
void modbus_cleanup(modbus_context_t* ctx);

/**
 * @brief Lê todos os registradores configurados
 * @param ctx Contexto Modbus
 * @param data Estrutura para armazenar os dados lidos
 * @return true se pelo menos uma leitura foi bem-sucedida, false caso contrário
 */
bool modbus_read_all(modbus_context_t* ctx, modbus_data_t* data);

/**
 * @brief Lê um registrador específico
 * @param ctx Contexto Modbus
 * @param address Endereço do registrador
 * @param value Ponteiro para armazenar o valor lido
 * @return true se leitura foi bem-sucedida, false caso contrário
 */
bool modbus_read_register(modbus_context_t* ctx, uint16_t address, uint16_t* value);

/**
 * @brief Imprime informações de configuração Modbus
 */
void modbus_print_config(void);

/**
 * @brief Imprime dados lidos de forma formatada
 * @param data Estrutura com os dados lidos
 */
void modbus_print_data(const modbus_data_t* data);

/**
 * @brief Converte valor para representação binária (0 ou 1)
 * @param value Valor a ser convertido
 * @return true se valor != 0, false se valor == 0
 */
bool modbus_value_to_binary(uint16_t value);

#endif // MODBUS_H
