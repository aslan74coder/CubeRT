.PHONY: clean All

All:
	@echo "----------Building project:[ crt - Debug ]----------"
	@"$(MAKE)" -f  "crt.mk"
clean:
	@echo "----------Cleaning project:[ crt - Debug ]----------"
	@"$(MAKE)" -f  "crt.mk" clean
