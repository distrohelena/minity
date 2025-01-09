/*
# _____        ____   ___
#   |     \/   ____| |___|
#   |     |   |   \  |   |
#-----------------------------------------------------------------------
# Copyright 2022, tyra - https://github.com/h4570/tyra
# Licensed under Apache License 2.0
# Sandro Sobczyński <sandro.sobczynski@gmail.com>
*/

#ifdef PS2
#include <tyra>
#include "tutorial_04.hpp"

namespace Tyra {

Tutorial04::Tutorial04(Engine* t_engine)
    : engine(t_engine) {}

Tutorial04::~Tutorial04() {
}

void Tutorial04::init() {
  core = new Core();
  core->Init(engine);
}

void Tutorial04::loop() {
  core->Draw();
}


}
#endif