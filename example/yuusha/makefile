.PHONY: mac build run clean

project_name=$(shell basename `pwd`)

mac:
	make -C raw

build:
	xcodebuild -project proj.mac/$(project_name).xcodeproj -target $(project_name)

run:
	./proj.mac/build/Release/$(project_name)

clean:
	rm -rf res/*
