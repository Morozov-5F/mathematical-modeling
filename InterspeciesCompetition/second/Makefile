include ../Makefile

data:
	@echo "Second case"
	@for i in $$(seq 1 10); do \
		N1=`echo "scale = 3; ($$i * 0.1)" | bc`; \
		N2=`echo "scale = 3; ($$i * 0.1 - 0.1)" | bc`; \
		R1=2; \
		A1=1; \
		B1=2; \
		R2=3; \
		A2=1; \
		B2=1; \
		$(TOPDIR)/$(TARGET) $(EXEC_FLAGS) -A $$A1 -B $$B1 -C $$R1 -D $$A2 -E $$B2 -F $$R2 -p $$N1 -P $$N2 -f "data_$$i.dat"; \
	done

pl:
	@echo "Plotting second target"
	@gnuplot plot.gp