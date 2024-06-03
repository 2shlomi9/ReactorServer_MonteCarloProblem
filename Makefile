# Top-level Makefile

SUBDIRS = q1 q2 q4

.PHONY: all $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	@echo Cleaning top-level files...
	@rm -rf *.o *.d *.out  # Remove any top-level build artifacts
	@for dir in $(SUBDIRS); do \
	  $(MAKE) -C $$dir clean; \
	done

.PHONY: clean
