SHELL := /bin/bash # Use bash syntax

.PHONY: compile
all: libbsn bodyhub sensornode

.PHONY: bodyhub
bodyhub:
	cd module/bodyhub && mkdir build && \
	cd build && mkdir output && cmake .. && \
	make

.PHONY: sensornode
sensornode:
	cd module/sensornode && mkdir build && \
	cd build && mkdir output && cmake .. && \
	make

.PHONY: libbsn
libbsn:
	cd libbsn && mkdir build && \
	cd build && cmake .. && make && \
	sudo make install

.PHONY: clean
clean:
	rm -rf */build
	rm -rf */*/build