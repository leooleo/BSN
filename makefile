.PHONY: compile
all: libbsn bodyhub sensornode

libbsn:
	cd libbsn && mkdir build && \ 
	cd build && cmake .. && make && \  
	sudo make instal

bodyhub:
	cd module/bodyhub && mkdir build && \
	cd build && mkdir output && cmake .. && \
	make

sensornode:
	cd module/sensornode && mkdir build && \
	cd build && mkdir output && cmake .. && \
	make


.PHONY: clean
clean:
	rm -rf */build
	rm -rf */*/build