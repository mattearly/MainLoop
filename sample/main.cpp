#include <MainLoop.h>
#include <iostream>
int main()
{
  static bool keepRunning = true;
  MainLoop::Get()->SetRunCondition([]() -> bool { return keepRunning; });
  MainLoop::Get()->AddToDelayedUpdate([]() {
    static int count = 0;
    count++;
    if (count > 10)
      keepRunning = false;
    std::cout << "update!\n"; 
    });
  MainLoop::Get()->Run();
  return 0;
}