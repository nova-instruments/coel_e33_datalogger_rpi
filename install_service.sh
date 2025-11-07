#!/bin/bash
#
# Script de instalação do serviço COEL E33 DataLogger
# Nova Instruments
#
# Este script configura o serviço systemd para executar
# a aplicação automaticamente na inicialização do sistema
#

set -e  # Parar em caso de erro

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configurações
SERVICE_NAME="coel-datalogger"
APP_NAME="app_armv6"
INSTALL_DIR="/opt/coel-datalogger"
CONFIG_FILE="config.txt"

echo -e "${BLUE}=== Instalador do Serviço COEL E33 DataLogger ===${NC}"
echo -e "${BLUE}Nova Instruments${NC}\n"

# Verificar se está rodando como root
if [ "$EUID" -ne 0 ]; then 
    echo -e "${RED}❌ Este script precisa ser executado como root (use sudo)${NC}"
    exit 1
fi

# Verificar se o executável existe no diretório atual
if [ ! -f "./$APP_NAME" ]; then
    echo -e "${RED}❌ Executável '$APP_NAME' não encontrado no diretório atual${NC}"
    echo -e "${YELLOW}   Certifique-se de executar este script no mesmo diretório do executável${NC}"
    exit 1
fi

echo -e "${GREEN}✅ Executável encontrado: $APP_NAME${NC}"

# Parar o serviço se já estiver rodando
if systemctl is-active --quiet $SERVICE_NAME; then
    echo -e "${YELLOW}⏸️  Parando serviço existente...${NC}"
    systemctl stop $SERVICE_NAME
fi

# Desabilitar o serviço se já estiver habilitado
if systemctl is-enabled --quiet $SERVICE_NAME 2>/dev/null; then
    echo -e "${YELLOW}🔓 Desabilitando serviço existente...${NC}"
    systemctl disable $SERVICE_NAME
fi

# Criar diretório de instalação
echo -e "${BLUE}📁 Criando diretório de instalação: $INSTALL_DIR${NC}"
mkdir -p $INSTALL_DIR

# Copiar executável
echo -e "${BLUE}📦 Copiando executável...${NC}"
cp -f ./$APP_NAME $INSTALL_DIR/$APP_NAME
chmod +x $INSTALL_DIR/$APP_NAME

# Copiar ou criar arquivo de configuração
if [ -f "./$CONFIG_FILE" ]; then
    echo -e "${GREEN}✅ Copiando arquivo de configuração existente${NC}"
    cp -f ./$CONFIG_FILE $INSTALL_DIR/$CONFIG_FILE
else
    echo -e "${YELLOW}⚠️  Arquivo de configuração não encontrado, criando padrão${NC}"
    echo "DEVICE_NAME=NI00002" > $INSTALL_DIR/$CONFIG_FILE
fi

# Criar arquivo de serviço systemd
echo -e "${BLUE}⚙️  Criando arquivo de serviço systemd...${NC}"
cat > /etc/systemd/system/$SERVICE_NAME.service << EOF
[Unit]
Description=COEL E33 DataLogger Service
Documentation=https://github.com/novainstruments
After=network.target

[Service]
Type=simple
User=root
WorkingDirectory=$INSTALL_DIR
ExecStart=$INSTALL_DIR/$APP_NAME
Restart=always
RestartSec=10
StandardOutput=journal
StandardError=journal

# Configurações de segurança
NoNewPrivileges=false
PrivateTmp=true

# Limites de recursos
LimitNOFILE=65536
LimitNPROC=4096

[Install]
WantedBy=multi-user.target
EOF

echo -e "${GREEN}✅ Arquivo de serviço criado: /etc/systemd/system/$SERVICE_NAME.service${NC}"

# Recarregar systemd
echo -e "${BLUE}🔄 Recarregando configuração do systemd...${NC}"
systemctl daemon-reload

# Habilitar serviço para iniciar no boot
echo -e "${BLUE}🔧 Habilitando serviço para iniciar automaticamente...${NC}"
systemctl enable $SERVICE_NAME

# Iniciar serviço
echo -e "${BLUE}🚀 Iniciando serviço...${NC}"
systemctl start $SERVICE_NAME

# Aguardar um momento para o serviço iniciar
sleep 2

# Verificar status
echo -e "\n${BLUE}📊 Status do serviço:${NC}"
systemctl status $SERVICE_NAME --no-pager -l

echo -e "\n${GREEN}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║  ✅ Instalação concluída com sucesso!                      ║${NC}"
echo -e "${GREEN}╚════════════════════════════════════════════════════════════╝${NC}\n"

echo -e "${BLUE}📋 Comandos úteis:${NC}"
echo -e "  ${YELLOW}Ver status:${NC}        sudo systemctl status $SERVICE_NAME"
echo -e "  ${YELLOW}Parar serviço:${NC}     sudo systemctl stop $SERVICE_NAME"
echo -e "  ${YELLOW}Iniciar serviço:${NC}   sudo systemctl start $SERVICE_NAME"
echo -e "  ${YELLOW}Reiniciar serviço:${NC} sudo systemctl restart $SERVICE_NAME"
echo -e "  ${YELLOW}Ver logs:${NC}          sudo journalctl -u $SERVICE_NAME -f"
echo -e "  ${YELLOW}Ver logs (últimas 50 linhas):${NC} sudo journalctl -u $SERVICE_NAME -n 50"
echo -e "  ${YELLOW}Desabilitar serviço:${NC} sudo systemctl disable $SERVICE_NAME"

echo -e "\n${BLUE}📁 Arquivos instalados em:${NC} $INSTALL_DIR"
echo -e "  ${YELLOW}Executável:${NC}     $INSTALL_DIR/$APP_NAME"
echo -e "  ${YELLOW}Configuração:${NC}  $INSTALL_DIR/$CONFIG_FILE"

echo -e "\n${BLUE}💡 Dica:${NC} Para editar o nome do dispositivo, edite o arquivo:"
echo -e "  ${YELLOW}sudo nano $INSTALL_DIR/$CONFIG_FILE${NC}"
echo -e "  Depois reinicie o serviço: ${YELLOW}sudo systemctl restart $SERVICE_NAME${NC}\n"

