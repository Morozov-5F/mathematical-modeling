include ../Makefile

data:
	@echo "Stable focus"
	@mkdir -p data
	@touch parameters.info
	@echo "w = 1\nr = 1\nJ = 0.5\nD = 1\nK = 3\nS = 1\n" > parameters.info
	@for i in $$(seq 0 10); do \
		for j in $$(seq 0 10); do \
			N1=`echo "scale = 3; ($$i * 0.5) + 0.1" | bc`; \
			N2=`echo "scale = 3; ($$j * 0.5) + 0.1" | bc`; \
			NUM=`echo "scale = 3; ($$i * 11 + $$j)" | bc`; \
			w=1; \
			r=1; \
			J=0.5; \
			D=1; \
			K=3; \
			S=1; \
			$(TOPDIR)/$(TARGET) $(EXEC_FLAGS) -w $$w -R $$r -J $$J -K $$K -D $$D -S $$S -p $$N1 -P $$N2 -f "data/data_$$NUM.dat"; \
		done; \
	done

clean_data:
	@echo "Cleaning data files for stable focus target"
	@$(RM) data

pl:
	@echo "Plotting stale focus case"
	@gnuplot plot.gp