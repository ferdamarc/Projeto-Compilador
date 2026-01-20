CC=gcc
CFLAGS=-I. -I./include
BUILD_DIR=./build
OUTPUT_DIR=./output
SRC_DIR=./src
TEST_DIR=./tests
UTILS_DIR=./utils
INPUTS_BINS_DIR=$(UTILS_DIR)/inputs_bins
FILES_DIR=$(TEST_DIR)/SO/PC4
SO_BIN=$(UTILS_DIR)/binSO.txt
RELOCATE=$(UTILS_DIR)/relocate
ROM_OUTPUT_DIR=$(UTILS_DIR)/output
ROM_OUTPUT_FILE=single_port_rom_init.txt

$(shell mkdir -p $(BUILD_DIR))
$(shell mkdir -p $(OUTPUT_DIR)/logs)
$(shell mkdir -p $(INPUTS_BINS_DIR))
$(shell mkdir -p $(ROM_OUTPUT_DIR))

clean:
	rm -rf $(BUILD_DIR)/*
	rm -f compiler

clean_output:
	rm -f $(OUTPUT_DIR)/logs/*
	rm -f $(OUTPUT_DIR)/logs/*.log
	rm -f $(OUTPUT_DIR)/*.txt

clean_bins:
	rm -f $(INPUTS_BINS_DIR)/*.txt
	rm -f $(ROM_OUTPUT_DIR)/$(ROM_OUTPUT_FILE)


compiler: $(BUILD_DIR)/lex.yy.o $(BUILD_DIR)/parser.tab.o $(BUILD_DIR)/codeGen.o $(BUILD_DIR)/main.o $(BUILD_DIR)/reg.o $(BUILD_DIR)/assembler.o $(BUILD_DIR)/tab.o $(BUILD_DIR)/memory.o $(BUILD_DIR)/label.o $(BUILD_DIR)/binario.o $(BUILD_DIR)/tree.o $(BUILD_DIR)/semantic.o $(BUILD_DIR)/funcoes_assembly.o
	$(CC) $(CFLAGS) -o $@ $^ -lfl

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/lex.yy.o: $(BUILD_DIR)/lex.yy.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/parser.tab.o: $(BUILD_DIR)/parser.tab.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/codeGen.o: $(SRC_DIR)/codeGen.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/reg.o: $(SRC_DIR)/reg.c $(wildcard $(BUILD_DIR)/*.h) $(wildcard ./include/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/memory.o: $(SRC_DIR)/memory.c $(wildcard $(BUILD_DIR)/*.h) $(wildcard ./include/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/label.o: $(SRC_DIR)/label.c $(wildcard $(BUILD_DIR)/*.h) $(wildcard ./include/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/funcoes_assembly.o: $(SRC_DIR)/funcoes_assembly.c $(wildcard $(BUILD_DIR)/*.h) $(wildcard ./include/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/tab.o: $(SRC_DIR)/tab.c $(wildcard $(BUILD_DIR)/*.h) $(wildcard ./include/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/binario.o: $(SRC_DIR)/binario.c $(wildcard $(BUILD_DIR)/*.h) $(wildcard ./include/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/lex.yy.c: $(SRC_DIR)/lexer.l $(BUILD_DIR)/parser.tab.h
	flex -o $@ $<

$(BUILD_DIR)/parser.tab.c $(BUILD_DIR)/parser.tab.h: $(SRC_DIR)/parser.y
	bison -d -v -t -Wcounterexamples $< -o $(BUILD_DIR)/parser.tab.c

$(BUILD_DIR)/assembler.o: $(SRC_DIR)/assembler.c $(wildcard $(BUILD_DIR)/*.h) $(wildcard ./include/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/tree.o: $(SRC_DIR)/tree.c $(wildcard $(BUILD_DIR)/*.h) $(wildcard ./include/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/semantic.o: $(SRC_DIR)/semantic.c $(wildcard $(BUILD_DIR)/*.h) $(wildcard ./include/*.h)
	$(CC) $(CFLAGS) -c $< -o $@


test: compiler
	@if [ -z "$(file)" ]; then \
		echo "Error: No test file specified. Usage: make test file=<file_path>"; \
		exit 1; \
	fi
	@echo "Running Single Test\n"
	@echo "Input file: $(file)"
	@log_file_name=$$(basename "$(file)").log; \
	log_file_path=$(OUTPUT_DIR)/logs/$$log_file_name; \
	echo "Attempting to save log to: $$log_file_path\n"; \
	./compiler "$(file)" -ca -ci -v > "$$log_file_path"; \
	if [ $$? -eq 0 ]; then \
		echo "Test output for $(file) saved to $$log_file_path\n"; \
	else \
		echo "Error running compiler for $(file). Check stderr for messages. Log might be empty or incomplete: $$log_file_path"; \
	fi
	@echo "Single Test End\n"


test_all: compiler
	@if [ -z "$(dir)" ]; then \
		echo "Error: No directory specified. Usage: make test_all dir=<directory_path>"; \
		exit 1; \
	fi
	@if [ ! -d "$(dir)" ]; then \
		echo "Error: Directory $(dir) not found."; \
		exit 1; \
	fi
	@echo "----------"
	@echo "Running All Tests in Directory\n"
	@echo "Target directory: $(dir)"
	@echo "Output directory for logs: $(OUTPUT_DIR)/logs \n"
	@for test_file in $(wildcard $(dir)/*); do \
		if [ -f "$$test_file" ]; then \
			echo "---"; \
			echo "Processing file: $$test_file \n"; \
			log_file_name=$$(basename "$$test_file").log; \
			log_file_path=$(OUTPUT_DIR)/logs/$$log_file_name; \
			echo "Attempting to save log to: $$log_file_path \n"; \
			./compiler "$$test_file" > "$$log_file_path"; \
			if [ $$? -eq 0 ]; then \
				echo "Test output for $$test_file saved to $$log_file_path"; \
			else \
				echo "Error running compiler for $$test_file. Check stderr for messages. Log might be empty ou incompleto: $$log_file_path \n"; \
			fi; \
		fi; \
	done
	@echo "----------"
	@echo "All Tests in Directory Ended\n"


# Compila o SO e salva em utils/binSO.txt
build_so: compiler
	@echo "----------"
	@echo "Building SO"
	@echo "Source file: $(FILES_DIR)/SO.cm"
	@echo "Output file: $(SO_BIN)\n"
	@if [ ! -f "$(FILES_DIR)/SO.cm" ]; then \
		echo "Error: SO.cm not found at $(FILES_DIR)/SO.cm"; \
		exit 1; \
	fi
	@./compiler "$(FILES_DIR)/SO.cm" > /dev/null 2>&1
	@if [ $$? -eq 0 ]; then \
		if [ -f "$(OUTPUT_DIR)/binario_final.txt" ]; then \
			cp $(OUTPUT_DIR)/binario_final.txt "$(SO_BIN)"; \
			echo "SO binary saved to $(SO_BIN)"; \
		else \
			echo "Error: binario_final.txt not generated for SO"; \
			exit 1; \
		fi; \
	else \
		echo "Compilation failed for SO.cm"; \
		exit 1; \
	fi
	@echo "----------"
	@echo "SO Built Successfully\n"


# Compila todos os programas do diretório PC4 (exceto SO) e salva os binários em utils/inputs_bins
build_pc4_bins: compiler
	@echo "----------"
	@echo "Building PC4 Programs"
	@echo "Source directory: $(FILES_DIR)"
	@echo "Output directory: $(INPUTS_BINS_DIR)\n"
	@mkdir -p $(INPUTS_BINS_DIR)
	@for test_file in $(FILES_DIR)/*.cm; do \
		if [ -f "$$test_file" ]; then \
			base_name=$$(basename "$$test_file" .cm); \
			if [ "$$base_name" = "SO" ]; then \
				echo "Skipping SO.cm (compiled separately)"; \
				continue; \
			fi; \
			output_bin=$(INPUTS_BINS_DIR)/$$base_name.txt; \
			echo "Compiling: $$test_file -> $$output_bin"; \
			./compiler "$$test_file" > /dev/null 2>&1; \
			if [ $$? -eq 0 ]; then \
				if [ -f "$(OUTPUT_DIR)/binario_final.txt" ]; then \
					cp $(OUTPUT_DIR)/binario_final.txt "$$output_bin"; \
					echo "  Binary saved to $$output_bin"; \
				else \
					echo "  Error: binario_final.txt not generated"; \
				fi; \
			else \
				echo "  Compilation failed for $$test_file"; \
			fi; \
		fi; \
	done
	@echo "----------"
	@echo "PC4 Programs Built Successfully\n"


# Gera a memória ROM concatenando SO + programas PC4
build_rom: build_so build_pc4_bins
	@echo "----------"
	@echo "Building ROM Memory"
	@if [ ! -f "$(RELOCATE)" ]; then \
		echo "Error: relocate executable not found at $(RELOCATE)"; \
		echo "Please compile relocate.c first with: make relocate"; \
		exit 1; \
	fi
	@if [ ! -f "$(SO_BIN)" ]; then \
		echo "Error: SO binary not found at $(SO_BIN)"; \
		echo "Please ensure the SO binary exists"; \
		exit 1; \
	fi
	@echo "Running relocate to generate ROM..."
	@echo "  Input dir: $(INPUTS_BINS_DIR)"
	@echo "  SO binary: $(SO_BIN)"
	@echo "  Output dir: $(ROM_OUTPUT_DIR)"
	@echo "  Output file: $(ROM_OUTPUT_FILE)"
	@$(RELOCATE) "$(INPUTS_BINS_DIR)" "$(SO_BIN)" "$(ROM_OUTPUT_DIR)" "$(ROM_OUTPUT_FILE)"
	@if [ $$? -eq 0 ]; then \
		echo "\nROM memory generated successfully at $(ROM_OUTPUT_DIR)/$(ROM_OUTPUT_FILE)"; \
	else \
		echo "\nError generating ROM memory"; \
		exit 1; \
	fi
	@echo "----------"
	@echo "ROM Build Completed\n"


# Atalho para compilar o relocate
relocate:
	@echo "----------"
	@echo "Compiling relocate"
	@$(CC) $(UTILS_DIR)/relocate.c -o $(RELOCATE)
	@if [ $$? -eq 0 ]; then \
		echo "Relocate compiled successfully at $(RELOCATE)"; \
	else \
		echo "Error compiling relocate"; \
		exit 1; \
	fi
	@echo "----------"
	@echo "Relocate Compilation Finished\n"

# Target completo: compila tudo e gera a ROM
all_rom: compiler relocate build_rom

	@echo "----------"
	@echo "Moving to Complete ROM Build from Directory of Quartus"
# Fazer isso após pegar o diretório de onde o arquivo binário do Quartus está sendo executado
	@echo "Complete ROM Build Finished"

