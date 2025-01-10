/*
# _____        ____   ___
#   |     \/   ____| |___|
#   |     |   |   \  |   |
#-----------------------------------------------------------------------
# Copyright 2022, tyra - https://github.com/h4570/tyra
# Licensed under Apache License 2.0
# Sandro Sobczyński <sandro.sobczynski@gmail.com>
*/

#pragma once

#ifdef PS2
#include <tyra>
// #include "state/state_manager.hpp"
#include "minity/Core.h"

using Tyra::Engine;
using Tyra::Game;

namespace Demo {

	class DemoGame : public Game {
	public:
		DemoGame(Engine* engine);
		~DemoGame();

		void init();
		void loop();

	private:
		Engine* engine;
		Core* core;
		// StateManager<GlobalStateType> stateManager;
	};

}  // namespace Demo
#endif