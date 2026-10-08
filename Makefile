OVERLAYS := item-trainer class-edit support-viewer
HOST_CXX ?= c++

.PHONY: all $(OVERLAYS) package test clean

all: $(OVERLAYS)

$(OVERLAYS):
	$(MAKE) -C $@

package: all
	python3 tools/package.py

test: .build-host/game-session
	./.build-host/game-session

.build-host/game-session: tests/game_session.cpp tests/include/switch.h common/common.hpp libs/Atmosphere-libs/libstratosphere/source/dmnt/dmntcht.h
	@mkdir -p .build-host
	$(HOST_CXX) -std=c++20 -Wall -Wextra -Itests/include -Ilibs/Atmosphere-libs/libstratosphere/source/dmnt $< -o $@

clean:
	$(foreach overlay,$(OVERLAYS),$(MAKE) -C $(overlay) clean;)
	rm -rf .build-host output
