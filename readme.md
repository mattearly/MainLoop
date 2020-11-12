# MainLoop

MainLoop, as the name might suggest, is a main loop built with C++. It includes calculating the delta time between updates for the programmer to use for calculations, timings, physics, or whatever you need delta time for. MainLoop can be dropped into any project as a single header.

## Examples

A Sample Text RPG Simulation using this loop

```cpp
#include "MainLoop.h"
#include <iostream>
#include <string>
int runDemoTestGame()
{
  struct Player {
    int str = 1;
    int hp = 1;
    int exp = 0;
  };
  static Player player;
  static std::string playername;
  static std::string action;
  static bool keepRunning;
  MainLoop::Get()->SetRunCondition([]() { return keepRunning; });
  MainLoop::Get()->AddToOnBegin([]() {
    keepRunning = true;
    std::cout << "Welcome to a new game of trainer.\n";
    std::cout << "Enter your character's name: ";
    std::getline(std::cin, playername);
    });
  MainLoop::Get()->AddToOnUpdate([]() {
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
  MainLoop::Get()->AddToOnPostUpdate([]() {
    std::cout << playername << ' ' << action << "s!\n";
    std::cout << " hp: " << player.hp << '\n';
    std::cout << " exp: " << player.exp << '\n';
    std::cout << " str: " << player.str << '\n';
    });
  return MainLoop::Get()->Run();
}
int main(){ return runDemoTestGame(); }
```
