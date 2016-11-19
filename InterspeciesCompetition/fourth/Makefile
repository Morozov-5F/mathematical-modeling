include ../Makefile

data:
	@echo "Fourth case"
	@for i in $$(seq 0 10); do \
		for j in $$(seq 0 10); do \
			N1=`echo "scale = 3; ($$i * 0.5)" | bc`; \
			N2=`echo "scale = 3; ($$j * 0.5)" | bc`; \
			NUM=`echo "scale = 3; ($$i * 11 + $$j)" | bc`; \
			R1=2; \
			A1=4; \
			B1=1; \
			R2=4; \
			A2=1; \
			B2=1; \
			$(TOPDIR)/$(TARGET) $(EXEC_FLAGS) -A $$A1 -B $$B1 -C $$R1 -D $$A2 -E $$B2 -F $$R2 -p $$N1 -P $$N2 -f "data_$$NUM.dat"; \
		done; \
	done

pl:
	@echo "Plotting fourth target"
	@gnuplot plot.gp