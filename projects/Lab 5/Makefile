all: main
flash: main
	@$(MAKE) -C main flash

main: library
	@$(MAKE) -C main all

library:
	@$(MAKE) -C libcolor all

clean:
	@$(MAKE) -C main clean
	@$(MAKE) -C libcolor clean