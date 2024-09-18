APPS := test

.PHONY: all build $(APPS) clean

all: build

build: $(APPS)

$(APPS):
	$(MAKE) -C $@ all
	mkdir -p bin
	mv $@/bin/* bin/

clean:
	for app in $(APPS); do \
		$(MAKE) -C $$app clean; \
	done
	rm -rf bin/*
