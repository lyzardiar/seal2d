dirs=$(shell ls -l | grep ^d | awk '{print $$9}')
dst_dir=../../res/images
tp=/Applications/TexturePacker.app/Contents/MacOS/Texturepacker
.PHONY: all $(dirs)

$(dirs): $(dirs)
	mkdir -p $(dst_dir)
	$(tp) --data $(dst_dir)/$@.json --format json --sheet $(dst_dir)/$@.png $@/*.png --trim-mode None --disable-rotation
	@echo making atlas in folder: $@

all:create_dir $(dirs)
