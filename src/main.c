/**
 * @file main.c
 * @brief COEL E33 DataLogger RPi - Main Application
 * @author Nova Instruments
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "modbus.h"
#include "datalogger.h"
#include "usb_manager.h"

// Configurações da aplicação
#define LOOP_INTERVAL_SECONDS 300  // 5 minutos = 300 segundos
#define DEVICE_NAME "NI00002"  // Nome do dispositivo - CONFIGURÁVEL

// Variável global para controle do loop principal
static volatile bool running = true;

/**
 * @brief Handler para sinais (SIGINT, SIGTERM)
 */
static void signal_handler(int sig) {
    printf("\nSinal %d recebido. Finalizando aplicação...\n", sig);
    running = false;
}

/**
 * @brief Configura handlers de sinais para saída graceful
 */
static void setup_signal_handlers(void) {
    signal(SIGINT, signal_handler);   // Ctrl+C
    signal(SIGTERM, signal_handler);  // Termination signal
}

/**
 * @brief Função principal da aplicação
 */
int main(void) {
    printf("=== COEL E33 DataLogger RPi ===\n");
    printf("Nova Instruments\n");
    printf("Dispositivo: %s\n\n", DEVICE_NAME);

    // Configurar handlers de sinais
    setup_signal_handlers();

    // Inicializar conexão Modbus
    modbus_context_t* modbus_ctx = modbus_init();
    if (!modbus_ctx) {
        fprintf(stderr, "Erro: Falha ao inicializar Modbus\n");
        return EXIT_FAILURE;
    }

    // Inicializar DataLogger
    datalogger_context_t* datalogger_ctx = datalogger_init(DEVICE_NAME);
    if (!datalogger_ctx) {
        fprintf(stderr, "Erro: Falha ao inicializar DataLogger\n");
        modbus_cleanup(modbus_ctx);
        return EXIT_FAILURE;
    }

    printf("\nIniciando loop de aquisição de dados (intervalo: %d segundos = %d minutos)\n",
           LOOP_INTERVAL_SECONDS, LOOP_INTERVAL_SECONDS / 60);
    printf("Pressione Ctrl+C para finalizar\n\n");

    // Loop principal de aquisição e logging
    // Estado anterior da porta (inicializar com valor inválido)
    bool previous_door_state_valid = false;
    uint16_t previous_door_state = 0;
    uint32_t door_change_logs = 0;

    // Controle de tempo para log periódico
    time_t last_periodic_log = time(NULL);

    while (running) {
        modbus_data_t data;
        bool should_log = false;
        bool is_door_change = false;

        printf("Lendo registradores Modbus...\n");

        if (modbus_read_all(modbus_ctx, &data)) {
            // Exibir dados na tela
            modbus_print_data(&data);

            // Verificar mudança de estado da porta
            if (data.valid_0x20d && previous_door_state_valid) {
                if (data.addr_0x20d != previous_door_state) {
                    should_log = true;
                    is_door_change = true;
                    printf("🚪 MUDANÇA DE ESTADO DA PORTA: %u → %u\n",
                           previous_door_state, data.addr_0x20d);
                }
            }

            // Verificar se é hora do log periódico (5 minutos)
            time_t current_time = time(NULL);
            if (!should_log && (current_time - last_periodic_log) >= LOOP_INTERVAL_SECONDS) {
                should_log = true;
                last_periodic_log = current_time;
                printf("⏰ Log periódico (5 minutos)\n");
            }

            // Registrar no datalogger se necessário
            if (should_log) {
                if (datalogger_log_data(datalogger_ctx, &data)) {
                    if (is_door_change) {
                        printf("✅ Mudança de porta registrada imediatamente no log\n");
                        door_change_logs++;
                    } else {
                        printf("✅ Dados registrados no log (periódico)\n");
                    }
                } else {
                    printf("❌ Erro ao registrar dados no log\n");
                }
            }

            // Atualizar estado anterior da porta
            if (data.valid_0x20d) {
                previous_door_state = data.addr_0x20d;
                previous_door_state_valid = true;
            }

        } else {
            printf("❌ Erro: Falha na leitura de todos os registradores\n");

            // Mesmo com erro, tentar registrar no log para manter histórico
            datalogger_log_data(datalogger_ctx, &data);
        }

        printf("----------------------------------------\n");

        // Aguardar próxima leitura (verificação mais frequente para detectar mudanças)
        // Verificar a cada 2 segundos em vez de 5 minutos
        for (int i = 0; i < 2 && running; i++) {
            sleep(1);
        }
    }

    // Cleanup
    printf("\nFinalizando aplicação...\n");

    // Mostrar estatísticas finais
    datalogger_print_stats(datalogger_ctx);
    printf("Mudanças de porta registradas: %u\n", door_change_logs);

    // Limpar recursos
    datalogger_cleanup(datalogger_ctx);
    modbus_cleanup(modbus_ctx);

    printf("Aplicação finalizada com sucesso.\n");
    return EXIT_SUCCESS;
}
