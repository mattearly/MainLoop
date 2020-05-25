/*
sample_runs.h
----------------------------------------------------------------------
Copyright (c) 2019-2020, Matthew Early matthewjearly@gmail.com
All rights reserved.
Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the
following conditions are met:
* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.
* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.
* Neither the name of the Matthew Early, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of Matthew Early.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
----------------------------------------------------------------------
*/
#pragma once
#include "../MainLoop/MainLoop.h"
#include <string>
#include <iostream>

void runDemoTestGame()
{
  /// DEMO CONSOLE GAME THING
  struct Player {
    int str = 1;
    int hp = 1;
    int exp = 0;
  };
  static Player player;
  static std::string playername;
  static std::string action;
  std::shared_ptr<MainLoop> ml = MainLoop::init();
  static bool keepRunning;
  ml->setRunCon([]() { return keepRunning; });
  ml->addToOnBegin([]() {
    keepRunning = true;
    std::cout << "Welcome to a new game of trainer.\n";
    std::cout << "Enter your character's name: ";
    std::getline(std::cin, playername);
    });
  ml->addToOnUpdate([]() {
    std::cout << "Enter an action('attack' or 'train' or 'quit'): ";
    std::getline(std::cin, action);
    if (action == "quit")
    {
      keepRunning = false;
    }
    else if (action == "attack")
    {
      if (player.str > 4)
        player.exp++;
      if (player.exp > player.str)
        player.hp++;
    }
    else if (action == "train")
    {
      player.str++;
    }
    });
  ml->addToOnPostUpdate([]() {
    std::cout << playername << ' ' << action << "s!\n";
    std::cout << " hp: " << player.hp << '\n';
    std::cout << " exp: " << player.exp << '\n';
    std::cout << " str: " << player.str << '\n';
    });
  ml->run();
}

void runAskHowMyDayWas()
{
  auto ml = MainLoop::init();
  ml->addToOnBegin([]() {std::cout << "Hello! (type 'quit' to exit\n"; });
  static std::string yourreply;
  static bool keepRunning;
  ml->addToOnBegin([]() {keepRunning = true; });
  ml->setRunCon([]() {return keepRunning; });
  ml->addToOnUpdate([]() {std::cout << "How was your day today?\nreply->"; std::getline(std::cin, yourreply); std::cout << "Well it is good to hear you are " << yourreply << " good luck with that.\n"; });
  ml->addToOnUpdate([]() {std::cout << std::string((yourreply.find("bad") != std::string::npos) ? "Hope it gets better soon! Stay strong my friend!\n" : ""); });
  ml->addToOnUpdate([]() { if (yourreply.find("quit") != std::string::npos) { keepRunning = false; } });
  ml->run();
}