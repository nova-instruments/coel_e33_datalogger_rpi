# COEL E33 DataLogger RPi

Sistema de aquisição de dados Modbus RTU para Raspberry Pi, desenvolvido para o projeto COEL E33.

## 📋 Descrição

Este projeto implementa um datalogger que realiza leitura de registradores Modbus via comunicação serial, especificamente projetado para funcionar em Raspberry Pi com cross-compilation para ARM.

### Características Principais

- 🔌 **Comunicação Modbus RTU** via porta serial (`/dev/serial0`)
- 🎯 **Cross-compilation** para ARM (Raspberry Pi 3/4)
- 📊 **Leitura de registradores** 0x200 e 0x20D
- 🔄 **Loop contínuo** com intervalo de 2 segundos
- 📱 **Deploy automatizado** via SSH

## 🛠️ Configuração do Ambiente

### Pré-requisitos

- Ubuntu/Debian (host de desenvolvimento)
- Cross-compiler ARM (`gcc-arm-linux-gnueabihf`)
- CMake 3.16+
- Raspberry Pi com UART habilitado

### Instalação Automática

```bash
# Configurar ambiente completo de cross-compilation
make setup

# Ou manualmente:
./scripts/setup_cross_compilation.sh
```

### Verificação do Ambiente

```bash
# Verificar se tudo está configurado
make check

# Ou manualmente:
./scripts/check_cross_compilation.sh
```

## 🔨 Compilação

### Usando Makefile (Recomendado)

```bash
# Compilar projeto
make build

# Limpar e recompilar
make rebuild

# Ver informações do projeto
make info
```

### Usando CMake Diretamente

```bash
# Configurar CMake
cmake -DCMAKE_TOOLCHAIN_FILE=./user_cross_compile_setup.cmake -B build-rpi -S .

# Compilar
make -C build-rpi -j$(nproc)
```

## 🚀 Deploy e Execução

### Deploy Automático

```bash
# Deploy para IP padrão (192.168.3.22)
make deploy

# Deploy para IP específico
make deploy RPI_IP=192.168.1.100 RPI_USER=pi
```

### Deploy Manual

```bash
# Transferir executável
./deploy_to_rpi.sh <IP_DA_RPI> <USUARIO>

# Ou via SCP
scp build-rpi/bin/app pi@<IP>:~/
```

### Execução na Raspberry Pi

```bash
# Executar com privilégios de root (necessário para acesso serial)
sudo ./app
```

## 📁 Estrutura do Projeto

```
coel_e33_datalogger_rpi/
├── src/                              # Código fonte principal
│   └── main.c                        # Aplicação principal
├── lib/                              # Bibliotecas do projeto
│   ├── usb_manager.c                 # Gerenciador USB
│   └── usb_manager.h                 # Headers USB
├── CMakeLists.txt                    # Configuração CMake
├── user_cross_compile_setup.cmake    # Toolchain ARM
├── Makefile                          # Comandos facilitados
├── deploy_to_rpi.sh                  # Script de deploy
├── scripts/                          # Scripts de build
│   ├── setup_cross_compilation.sh    # Configuração completa
│   ├── check_cross_compilation.sh    # Verificação do ambiente
│   ├── build_libmodbus_arm.sh        # Build libmodbus ARM
│   ├── build_libgpiod_arm.sh         # Build libgpiod ARM
│   └── build_libudev_arm.sh          # Build libudev ARM
└── deps/                             # Dependências compiladas (ignorado no Git)
    ├── libmodbus/
    ├── libgpiod/
    └── eudev/
```

## ⚙️ Configuração da Raspberry Pi

### Habilitar UART

Adicionar ao `/boot/config.txt`:
```
enable_uart=1
dtoverlay=disable-bt
```

### Verificar Dispositivo Serial

```bash
# Verificar se /dev/serial0 existe
ls -la /dev/serial*

# Verificar configuração UART
dmesg | grep tty
```

## 📊 Dados Lidos

O sistema lê dois registradores Modbus:

- **0x200**: Valor numérico (16-bit)
- **0x20D**: Valor binário (0 ou 1)

### Exemplo de Saída

```
Iniciando leitura Modbus...
Dispositivo: /dev/serial0
Configuração: 9600-N-8-1
Slave ID: 1
Endereços: 0x200 (0x200) e 0x20D (0x20D)
----------------------------------------
Conexão estabelecida com sucesso!

Lendo registradores...
Endereço 0x200: 1234 (0x04D2)
Endereço 0x20D: 1 (0x0001) - Binário: 1
----------------------------------------
```

## 🧪 Testes

```bash
# Executar verificações básicas
make test

# Verificar se executável é ARM
file build-rpi/bin/app
```

## 🔧 Comandos Úteis

```bash
# Ver todos os comandos disponíveis
make help

# Informações do projeto
make info

# Status do ambiente
make check
```

## 📝 Configurações Modbus

- **Dispositivo**: `/dev/serial0`
- **Baud Rate**: 9600
- **Paridade**: Nenhuma (N)
- **Data Bits**: 8
- **Stop Bits**: 1
- **Slave ID**: 1
- **Timeout**: 500ms (resposta), 200ms (byte)

## 🤝 Contribuição

1. Fork o projeto
2. Crie uma branch para sua feature (`git checkout -b feature/nova-feature`)
3. Commit suas mudanças (`git commit -am 'Adiciona nova feature'`)
4. Push para a branch (`git push origin feature/nova-feature`)
5. Abra um Pull Request

## 📄 Licença

Este projeto é propriedade da Nova Instruments.

## 🏢 Desenvolvido por

**Nova Instruments**  
Sistema de DataLogger COEL E33
