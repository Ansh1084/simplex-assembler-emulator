CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

ASM = assembler
EMU = emulator

# ==========================
# CHANGE THIS LINE ONLY
# ==========================
TEST_DIR = tests/test1.asm
# TEST_DIR = failure
# TEST_DIR = Tests

TEST_FILES := $(wildcard $(TEST_DIR)/*.asm)

all: $(ASM) $(EMU)

$(ASM): asm.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(EMU): emu.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(ASM) $(EMU) *.o *.log *.lst

# ==========================
# AUTOMATIC TEST RUNNER
# ==========================

test: all
	@echo "===================================="
	@echo " Running tests from $(TEST_DIR)/"
	@echo "===================================="
	@for file in $(TEST_FILES); do \
		echo "Testing $$file"; \
		./$(ASM) $$file >/dev/null 2>&1; \
		obj=$${file%.asm}.o; \
		if [ ! -f "$$obj" ]; then \
			echo "❌ Assembler FAILED: $$file"; \
			continue; \
		fi; \
		./$(EMU) $$obj; \
		if [ $$? -eq 0 ]; then \
			echo "✅ PASS: $$file"; \
		else \
			echo "❌ Emulator FAILED: $$file"; \
		fi; \
	done
	@echo "===================================="
	@echo " All tests completed"
	@echo "===================================="