dirs=$(shell ls -l | grep ^d | awk '{print $$9}')
dst_dir=../res
.PHONY: all $(dirs)
all: $(dirs)

$(dirs):
	make -C $@

