dirs=$(shell ls -l | grep ^d | awk '{print $$9}')
dst_dir=../../res
tp=/Applications/TexturePacker.app/Contents/MacOS/Texturepacker
.PHONY: all $(dirs)

$(dirs):
	mkdir -p $(dst_dir)
	$(tp) --data $(dst_dir)/$@.lua --format corona --sheet $(dst_dir)/$@.png $@/*.png
	@echo making atlas in folder: $@

all:create_dir $(dirs)
