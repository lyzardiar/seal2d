dirs=$(shell ls -l | grep ^d | awk '{print $$9}')
.PHONY: all $(dirs)
all: $(dirs)

$(dirs):
	make -C $@

