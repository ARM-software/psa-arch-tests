.PHONY: all clean

# Test specific macros
OUT_DIR:=./out

# Root folder
ROOT_DIR:=.

DIRECTORY := $(dir $(wildcard $(ROOT_DIR)/*/))
DIRECTORY := $(filter-out ./, $(DIRECTORY))
DIRECTORY := $(filter-out $(OUT_DIR)/, $(DIRECTORY))
DIRECTORY := $(sort $(filter-out ./include/, $(DIRECTORY)))
CLEAN_DIRECTORY :=$(DIRECTORY)

all: build output
	./tbsa_elf_combine.pl $(ROOT_DIR)/
	mv ./tbsa_test_combined.bin $(OUT_DIR)/.
	hexdump -v -e ' 1/4 "%08X" "\n"' $(OUT_DIR)/tbsa_test_combined.bin > $(OUT_DIR)/tbsa_test_combined.hex

output:
	mkdir -p $(OUT_DIR)

build:
	$(foreach d, $(DIRECTORY), cd $d && $(MAKE) && cd ..;)

clean:
	$(foreach d, $(CLEAN_DIRECTORY), cd $d && $(MAKE) clean && cd ..;)
	rm -rf $(OUT_DIR)
