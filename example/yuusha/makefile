.PHONY: test coverage run mac ios android clean

project_name=$(shell basename `pwd`)
test:

coverage: test
	luacov

mac:
	make -C raw
	xcodebuild -project proj.mac/$(project_name).xcodeproj -target $(project_name)

run:
	./proj.mac/build/Release/$(project_name)

clean:
	rm -rf res/*