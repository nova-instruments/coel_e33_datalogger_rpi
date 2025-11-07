/**
 * @file relay_control.h
 * @brief COEL E33 DataLogger - Relay Control Library
 * @author Nova Instruments
 * 
 * Controla 2 relés:
 * - GPIO 24 (pino físico 18): Lâmpada
 * - GPIO 25 (pino físico 22): Discadora
 */

#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Definições dos GPIOs dos relés
#define RELAY_LAMP_GPIO     24  // Relé da lâmpada
#define RELAY_DIALER_GPIO   25  // Relé da discadora

/**
 * @brief Inicializa o controle dos relés
 * @return 0 em caso de sucesso, -1 em caso de erro
 */
int relay_init(void);

/**
 * @brief Finaliza o controle dos relés e libera recursos
 */
void relay_cleanup(void);

/**
 * @brief Liga a lâmpada (relé GPIO 24)
 * @return 0 em caso de sucesso, -1 em caso de erro
 */
int relay_lamp_on(void);

/**
 * @brief Desliga a lâmpada (relé GPIO 24)
 * @return 0 em caso de sucesso, -1 em caso de erro
 */
int relay_lamp_off(void);

/**
 * @brief Liga a discadora (relé GPIO 25)
 * @return 0 em caso de sucesso, -1 em caso de erro
 */
int relay_dialer_on(void);

/**
 * @brief Desliga a discadora (relé GPIO 25)
 * @return 0 em caso de sucesso, -1 em caso de erro
 */
int relay_dialer_off(void);

/**
 * @brief Controla a lâmpada baseado no estado da porta
 * @param door_open true se porta aberta (1), false se fechada (0)
 * @return 0 em caso de sucesso, -1 em caso de erro
 */
int relay_control_lamp_by_door(bool door_open);

#ifdef __cplusplus
}
#endif

#endif // RELAY_CONTROL_H

