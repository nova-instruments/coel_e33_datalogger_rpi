# Makefile para o projeto Modbus Reader
# Facilita o uso dos scripts de build e deploy

.PHONY: help setup build clean check deploy test

# Configurações
BUILD_DIR = build-rpi
RPI_IP ?= 192.168.3.22
RPI_USER ?= nova

help:
	@echo "=== Modbus Reader - Comandos Disponíveis ==="
	@echo ""
	@echo "📋 Configuração:"
	@echo "  make setup     - Configura ambiente de cross-compilation"
	@echo "  make check     - Verifica se ambiente está configurado"
	@echo ""
	@echo "🔨 Compilação:"
	@echo "  make build     - Compila o projeto para ARM"
	@echo "  make clean     - Limpa arquivos de build"
	@echo "  make rebuild   - Limpa e recompila"
	@echo ""
	@echo "🚀 Deploy:"
	@echo "  make deploy    - Envia para Raspberry Pi (IP=$(RPI_IP), USER=$(RPI_USER))"
	@echo "  make deploy RPI_IP=<ip> RPI_USER=<user> - Deploy com IP/usuário específicos"
	@echo ""
	@echo "🧪 Testes:"
	@echo "  make test      - Executa verificações básicas"
	@echo ""
	@echo "📖 Informações:"
	@echo "  make info      - Mostra informações do projeto"

setup:
	@echo "🔧 Configurando ambiente de cross-compilation..."
	@chmod +x scripts/setup_cross_compilation.sh
	@./scripts/setup_cross_compilation.sh

check:
	@echo "🔍 Verificando ambiente..."
	@chmod +x scripts/check_cross_compilation.sh
	@./scripts/check_cross_compilation.sh

build:
	@echo "🔨 Compilando projeto..."
	@if [ ! -d "$(BUILD_DIR)" ]; then \
		echo "⚠️  Diretório de build não existe. Execute 'make setup' primeiro."; \
		exit 1; \
	fi
	@make -C $(BUILD_DIR) -j$$(nproc)
	@echo "✅ Compilação concluída!"
	@echo "📁 Executável: $(BUILD_DIR)/bin/app"

clean:
	@echo "🧹 Limpando arquivos de build..."
	@if [ -d "$(BUILD_DIR)" ]; then \
		rm -rf $(BUILD_DIR); \
		echo "✅ Diretório $(BUILD_DIR) removido"; \
	else \
		echo "ℹ️  Nada para limpar"; \
	fi

rebuild: clean setup build

deploy:
	@echo "🚀 Fazendo deploy para Raspberry Pi..."
	@if [ ! -f "$(BUILD_DIR)/bin/app" ]; then \
		echo "❌ Executável não encontrado. Execute 'make build' primeiro."; \
		exit 1; \
	fi
	@chmod +x deploy_to_rpi.sh
	@./deploy_to_rpi.sh $(RPI_IP) $(RPI_USER)

test: check
	@echo "🧪 Executando testes básicos..."
	@if [ -f "$(BUILD_DIR)/bin/app" ]; then \
		echo "✅ Executável existe"; \
		file $(BUILD_DIR)/bin/app; \
		ls -lh $(BUILD_DIR)/bin/app; \
	else \
		echo "❌ Executável não encontrado"; \
		exit 1; \
	fi

info:
	@echo "=== Informações do Projeto Modbus Reader ==="
	@echo ""
	@echo "📁 Estrutura:"
	@echo "  • src/main.c               - Código principal"
	@echo "  • lib/usb_manager.*        - Biblioteca USB"
	@echo "  • CMakeLists.txt           - Configuração CMake"
	@echo "  • user_cross_compile_setup.cmake - Toolchain ARM"
	@echo "  • scripts/                 - Scripts de build"
	@echo "  • deps/                    - Dependências compiladas"
	@echo ""
	@echo "🔧 Dependências:"
	@echo "  • libmodbus $(shell [ -f deps/libmodbus/install/lib/libmodbus.so ] && echo '✅' || echo '❌')"
	@echo "  • libgpiod  $(shell [ -f deps/libgpiod/install/lib/libgpiod.so ] && echo '✅' || echo '❌')"
	@echo "  • libudev   $(shell [ -f deps/eudev/install/lib/libudev.a ] && echo '✅' || echo '❌')"
	@echo "  • sqlite3   $(shell [ -f deps/sqlite3/install/lib/libsqlite3.so ] && echo '✅' || echo '❌')"
	@echo ""
	@echo "🎯 Alvo: Raspberry Pi 3 (ARM Cortex-A53)"
	@echo "📡 Protocolo: Modbus RTU via RS-485"
	@echo ""
	@if [ -f "$(BUILD_DIR)/bin/app" ]; then \
		echo "📦 Executável: ✅ $(BUILD_DIR)/bin/app"; \
		echo "📏 Tamanho: $$(ls -lh $(BUILD_DIR)/bin/app | awk '{print $$5}')"; \
	else \
		echo "📦 Executável: ❌ Não compilado"; \
	fi

# Atalhos convenientes
configure: setup
compile: build
install: deploy
status: info
verify: check
