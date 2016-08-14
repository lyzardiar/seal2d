.PHONY: all clean
dst_dir = ../../res/scores
xmls := $(shell ls *.xml )
jsons := $(patsubst %.xml, %.json, $(xmls))

%.json : %.xml
	xml2json < $^ > $@
	mkdir -p $(dst_dir) && mv *.json $(dst_dir)

all: clean $(jsons) 

clean:
	rm -rf *.json
