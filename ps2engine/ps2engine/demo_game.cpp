/*
# _____        ____   ___
#   |     \/   ____| |___|
#   |     |   |   \  |   |
#-----------------------------------------------------------------------
# Copyright 2022, tyra - https://github.com/h4570/tyra
# Licensed under Apache License 2.0
# Sandro Sobczyński <sandro.sobczynski@gmail.com>
*/

#include "demo_game.hpp"
#ifdef PS2

namespace Demo {

	DemoGame::DemoGame(Engine* t_engine) : engine(t_engine) {}
	DemoGame::~DemoGame() {}

	void DemoGame::init() {
		core = new Core();
		core->Init(engine);
	}
	void DemoGame::loop() {
		core->Draw();

		// engine->renderer.beginFrame();

		// engine->renderer.endFrame();

	}

}  // namespace Demo
#endif