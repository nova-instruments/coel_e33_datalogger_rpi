#ifndef USB_MANAGER_H
#define USB_MANAGER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Estrutura para informações do dispositivo USB (declaração pública)
typedef struct {
    char device_path[256];
    char mount_point[256];
    char fs_type[32];
    unsigned long size_mb;
    bool is_mounted;
    char vendor[64];
    char model[64];
} usb_device_info_t;

// Códigos de retorno
typedef enum {
    USB_SUCCESS = 0,
    USB_ERROR_INIT = -1,
    USB_ERROR_NOT_FOUND = -2,
    USB_ERROR_MOUNT_FAILED = -3,
    USB_ERROR_COPY_FAILED = -4,
    USB_ERROR_INVALID_PARAM = -5
} usb_result_t;

// Estrutura para callback de progresso
typedef struct {
    void (*on_progress)(int percentage, const char* message);
    void (*on_complete)(usb_result_t result, const char* message);
    void (*on_error)(usb_result_t error, const char* message);
} usb_callbacks_t;

/**
 * @brief Inicializa o gerenciador USB
 * @return 0 em caso de sucesso, -1 em caso de erro
 */
int usb_manager_init(void);

/**
 * @brief Finaliza o gerenciador USB e libera recursos
 */
void usb_manager_cleanup(void);

/**
 * @brief Detecta dispositivos USB removíveis conectados
 * @param devices Array para armazenar informações dos dispositivos encontrados
 * @param max_devices Número máximo de dispositivos a serem detectados
 * @return Número de dispositivos encontrados, ou -1 em caso de erro
 */
int detect_usb_devices(usb_device_info_t* devices, int max_devices);

/**
 * @brief Monta um dispositivo USB automaticamente
 * @param device_info Informações do dispositivo a ser montado
 * @return 0 em caso de sucesso, código de erro negativo caso contrário
 */
int mount_usb_device_auto(usb_device_info_t* device_info);

/**
 * @brief Desmonta um dispositivo USB
 * @param mount_point Ponto de montagem do dispositivo
 * @return 0 em caso de sucesso, código de erro negativo caso contrário
 */
int unmount_usb_device(const char* mount_point);

/**
 * @brief Copia arquivo de log para dispositivo USB
 * @param usb_device Informações do dispositivo USB de destino
 * @param log_file_path Caminho do arquivo de log a ser copiado
 * @param callbacks Callbacks para notificação de progresso (pode ser NULL)
 * @return 0 em caso de sucesso, código de erro negativo caso contrário
 */
int copy_log_to_usb(const usb_device_info_t* usb_device, const char* log_file_path, const usb_callbacks_t* callbacks);

/**
 * @brief Função principal para extrair logs para USB
 * Esta função detecta automaticamente um USB, monta se necessário e copia o arquivo
 * @param callbacks Callbacks para notificação de progresso (pode ser NULL)
 * @return 0 em caso de sucesso, código de erro negativo caso contrário
 */
int extract_log_to_usb(const usb_callbacks_t* callbacks);

/**
 * @brief Verifica se há espaço suficiente no USB para o arquivo
 * @param mount_point Ponto de montagem do USB
 * @param file_size Tamanho do arquivo em bytes
 * @return true se há espaço suficiente, false caso contrário
 */
bool check_usb_space(const char* mount_point, unsigned long file_size);

/**
 * @brief Obtém informações de espaço livre no USB
 * @param mount_point Ponto de montagem do USB
 * @param free_space_mb Ponteiro para armazenar espaço livre em MB
 * @param total_space_mb Ponteiro para armazenar espaço total em MB
 * @return 0 em caso de sucesso, -1 em caso de erro
 */
int get_usb_space_info(const char* mount_point, unsigned long* free_space_mb, unsigned long* total_space_mb);

/**
 * @brief Limpa pontos de montagem órfãos e desmonta USBs não utilizados
 * @return Número de pontos de montagem processados
 */
int cleanup_orphaned_mount_points(void);

/**
 * @brief Força a desmontagem de todos os dispositivos USB montados
 * @return Número de dispositivos desmontados
 */
int force_unmount_all_usb(void);

#ifdef __cplusplus
}
#endif

#endif // USB_MANAGER_H
