/**
 * @file modbus.c
 * @brief COEL E33 DataLogger - Modbus RTU Library Implementation
 * @author Nova Instruments
 */

#include "modbus.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <modbus/modbus.h>

// Estrutura interna do contexto Modbus
struct modbus_context_s {
    void* ctx;  // modbus_t* - usando void* para evitar dependência circular
    bool connected;
};

/**
 * @brief Função auxiliar para tratamento de erros
 */
static void modbus_error(modbus_context_t* mb_ctx, const char* msg) {
    fprintf(stderr, "Erro Modbus: %s: %s\n", msg, modbus_strerror(errno));
    if (mb_ctx && mb_ctx->ctx) {
        modbus_close((modbus_t*)mb_ctx->ctx);
        modbus_free((modbus_t*)mb_ctx->ctx);
        mb_ctx->ctx = NULL;
        mb_ctx->connected = false;
    }
}

modbus_context_t* modbus_init(void) {
    modbus_context_t* mb_ctx = malloc(sizeof(struct modbus_context_s));
    if (!mb_ctx) {
        fprintf(stderr, "Erro: Falha ao alocar memória para contexto Modbus\n");
        return NULL;
    }

    mb_ctx->ctx = NULL;
    mb_ctx->connected = false;

    printf("Iniciando conexão Modbus...\n");
    modbus_print_config();

    // Criar contexto RTU
    mb_ctx->ctx = (void*)modbus_new_rtu(MODBUS_DEVICE, MODBUS_BAUD_RATE,
                                        MODBUS_PARITY, MODBUS_DATA_BITS, MODBUS_STOP_BITS);
    if (!mb_ctx->ctx) {
        modbus_error(mb_ctx, "Erro ao criar contexto Modbus");
        free(mb_ctx);
        return NULL;
    }

    // Definir ID do escravo
    if (modbus_set_slave((modbus_t*)mb_ctx->ctx, MODBUS_SLAVE_ID) == -1) {
        modbus_error(mb_ctx, "Erro ao definir slave ID");
        free(mb_ctx);
        return NULL;
    }

    // Configurar timeouts
    modbus_set_response_timeout((modbus_t*)mb_ctx->ctx, 0, MODBUS_RESPONSE_TIMEOUT_US);
    modbus_set_byte_timeout((modbus_t*)mb_ctx->ctx, 0, MODBUS_BYTE_TIMEOUT_US);

    // Abrir conexão
    if (modbus_connect((modbus_t*)mb_ctx->ctx) == -1) {
        modbus_error(mb_ctx, "Erro na conexão");
        free(mb_ctx);
        return NULL;
    }

    mb_ctx->connected = true;
    printf("Conexão Modbus estabelecida com sucesso!\n\n");

    return mb_ctx;
}

void modbus_cleanup(modbus_context_t* ctx) {
    if (!ctx) return;

    if (ctx->ctx) {
        if (ctx->connected) {
            modbus_close((modbus_t*)ctx->ctx);
        }
        modbus_free((modbus_t*)ctx->ctx);
    }

    free(ctx);
}

bool modbus_read_register(modbus_context_t* ctx, uint16_t address, uint16_t* value) {
    if (!ctx || !ctx->ctx || !ctx->connected || !value) {
        return false;
    }

    int rc = modbus_read_registers((modbus_t*)ctx->ctx, address, 1, value);
    if (rc == -1) {
        fprintf(stderr, "Erro ao ler endereço 0x%X: %s\n", address, modbus_strerror(errno));
        return false;
    }

    return true;
}

bool modbus_read_all(modbus_context_t* ctx, modbus_data_t* data) {
    if (!ctx || !data) {
        return false;
    }

    // Inicializar estrutura
    memset(data, 0, sizeof(modbus_data_t));

    // Ler registrador 0x200
    data->valid_0x200 = modbus_read_register(ctx, MODBUS_ADDR_0x200, &data->addr_0x200);

    // Ler registrador 0x20D
    data->valid_0x20d = modbus_read_register(ctx, MODBUS_ADDR_0x20D, &data->addr_0x20d);
    
    // Converter 0x20D para binário
    if (data->valid_0x20d) {
        data->addr_0x20d_binary = modbus_value_to_binary(data->addr_0x20d);
    }

    // Retorna true se pelo menos uma leitura foi bem-sucedida
    return (data->valid_0x200 || data->valid_0x20d);
}

void modbus_print_config(void) {
    printf("Configuração Modbus:\n");
    printf("  Dispositivo: %s\n", MODBUS_DEVICE);
    printf("  Configuração: %d-%c-%d-%d\n", MODBUS_BAUD_RATE, MODBUS_PARITY, 
           MODBUS_DATA_BITS, MODBUS_STOP_BITS);
    printf("  Slave ID: %d\n", MODBUS_SLAVE_ID);
    printf("  Endereços: 0x%X e 0x%X\n", MODBUS_ADDR_0x200, MODBUS_ADDR_0x20D);
    printf("  Timeout resposta: %d ms\n", MODBUS_RESPONSE_TIMEOUT_US / 1000);
    printf("  Timeout byte: %d ms\n", MODBUS_BYTE_TIMEOUT_US / 1000);
    printf("----------------------------------------\n");
}

void modbus_print_data(const modbus_data_t* data) {
    if (!data) return;

    printf("Dados lidos:\n");
    
    if (data->valid_0x200) {
        float temp_celsius = data->addr_0x200 / 10.0f;
        printf("  Endereço 0x200: %u (0x%04X) = %.1f°C\n", data->addr_0x200, data->addr_0x200, temp_celsius);
    } else {
        printf("  Endereço 0x200: ERRO na leitura\n");
    }

    if (data->valid_0x20d) {
        printf("  Endereço 0x20D: %u (0x%04X) - Binário: %s\n", 
               data->addr_0x20d, data->addr_0x20d, 
               data->addr_0x20d_binary ? "1" : "0");
    } else {
        printf("  Endereço 0x20D: ERRO na leitura\n");
    }
}

bool modbus_value_to_binary(uint16_t value) {
    return (value != 0);
}
