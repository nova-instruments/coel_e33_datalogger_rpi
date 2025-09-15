// main.c - COEL E33 DataLogger RPi
 #include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <modbus/modbus.h>

#define DEVICE     "/dev/serial0" 
#define BAUD_RATE  9600
#define PARITY 'N'
#define DATA_BITS  8
#define STOP_BITS  1
#define SLAVE_ID   1

// Endereços Modbus a serem lidos
#define ADDR_0x200 0x200
#define ADDR_0x20D 0x20D

static void die(modbus_t *ctx, const char *msg) {
    fprintf(stderr, "%s: %s\n", msg, modbus_strerror(errno));
    if (ctx) {
        modbus_close(ctx);
        modbus_free(ctx);
    }
    exit(EXIT_FAILURE);
}

int main(void) {
    modbus_t *ctx = NULL;
    uint16_t tab_reg[2];
    int rc;

    printf("Iniciando leitura Modbus...\n");
    printf("Dispositivo: %s\n", DEVICE);
    printf("Configuração: %d-%c-%d-%d\n", BAUD_RATE, PARITY, DATA_BITS, STOP_BITS);
    printf("Slave ID: %d\n", SLAVE_ID);
    printf("Endereços: 0x%X (0x200) e 0x%X (0x20D)\n", ADDR_0x200, ADDR_0x20D);
    printf("----------------------------------------\n");

    // 1) Cria contexto RTU
    ctx = modbus_new_rtu(DEVICE, BAUD_RATE, PARITY, DATA_BITS, STOP_BITS);
    if (!ctx) die(NULL, "Erro ao criar contexto Modbus");

    // 2) Define ID do escravo
    if (modbus_set_slave(ctx, SLAVE_ID) == -1)
        die(ctx, "Erro ao definir slave ID");

    // 3) Ajusta timeouts antes do connect
    modbus_set_response_timeout(ctx, 0, 500000); // 500 ms
    modbus_set_byte_timeout(ctx, 0, 200000);     // 200 ms por byte

    // 4) Abre a porta serial
    if (modbus_connect(ctx) == -1)
        die(ctx, "Erro na conexão");

    printf("Conexão estabelecida com sucesso!\n\n");

    // 5) Loop principal de leitura
    while (1) {
        printf("Lendo registradores...\n");

        rc = modbus_read_registers(ctx, ADDR_0x200, 1, &tab_reg[0]);
        if (rc == -1) {
            fprintf(stderr, "Erro ao ler endereço 0x200: %s\n", modbus_strerror(errno));
        } else {
            printf("Endereço 0x200: %u (0x%04X)\n", tab_reg[0], tab_reg[0]);
        }

        rc = modbus_read_registers(ctx, ADDR_0x20D, 1, &tab_reg[1]);
        if (rc == -1) {
            fprintf(stderr, "Erro ao ler endereço 0x20D: %s\n", modbus_strerror(errno));
        } else {
            printf("Endereço 0x20D: %u (0x%04X) - Binário: %s\n", tab_reg[1], tab_reg[1],
                   (tab_reg[1] == 0) ? "0" : "1");
        }

        printf("----------------------------------------\n");
        sleep(2);
    }

    // (não alcançado neste exemplo)
    modbus_close(ctx);
    modbus_free(ctx);
    return 0;
}
