# MainLoop

MainLoop, as the name might suggest, is a main loop built with C++. It includes calculating the delta time between updates for the programmer use, which is always nice for consistency across platforms in games and physics. MainLoop can be dropped into any project (submodule it and add it to your cmake project is what I would suggest). It can be built in Release or Debug with either x86 or x64 on any OS. This is a static library (for now). There is no reason it can't be a dynamic library with some changes but I haven't tested it as a dynamic library yet.

## Run Tests

~In Visual Studio, go to Tests > RunAllTests~

## Run Demos

~Set one of the Demo projects as a the startup project and run.~

## Examples

Sample Code Making A Text RPG using this loop
````
void runDemoTestGame()
{
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
int main(){ runDemoTestGame(); }
````

Sample Code Making a Console Game with Graphics (thanks OneLoneCoder for the tutorial on this, I just put it in this MainLoop instead of using while(1))
````
int screenWidth = 120;
int screenHeight = 40;
int mapWidth = 16;
int mapHeight = 16;
float playerX = 14.7f;
float playerY = 5.09f;
float playerSpeed = .5f;
float playerAngle = 0.f;
float playerFOV = 3.14159f / 4.f;
float playerViewDepth = 16.f;

/// screen buffer
wchar_t* consoleScreen = new wchar_t[screenWidth * screenHeight];
HANDLE consoleHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
DWORD winBytesWritten = 0;
std::wstring mainMap;

bool isRunning;
float fps;

int main()
{
  SetConsoleActiveScreenBuffer(consoleHandle);

  mainMap += L"################";
  mainMap += L"#..............#";
  mainMap += L"#.......###....#";
  mainMap += L"#.......#......#";
  mainMap += L"#.......#......#";
  mainMap += L"#.......#......#";
  mainMap += L"#..............#";
  mainMap += L"#######........#";
  mainMap += L"#..............#";
  mainMap += L"#............###";
  mainMap += L"#..............#";
  mainMap += L"#..............#";
  mainMap += L"#..............#";
  mainMap += L"#......###.##..#";
  mainMap += L"#..............#";
  mainMap += L"################";

  auto mainLoop = MainLoop::get();
  mainLoop->setRunCon([]() {return isRunning; });
  mainLoop->addToOnBegin([]() {isRunning = true; });

  mainLoop->addToOnUpdate([](float dt) {
    // a press
    if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
    {
      playerAngle -= (playerSpeed * .75f) * dt;
    }
    // d press
    if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
    {
      playerAngle += (playerSpeed * .75f) * dt;
    }

    // w press
    if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
    {
      playerX += sinf(playerAngle) * playerSpeed * dt;
      playerY += cosf(playerAngle) * playerSpeed * dt;

      if (mainMap.c_str()[(int)playerX * mapWidth * (int)playerY] == '#')
      {
        playerX -= sinf(playerAngle) * playerSpeed * dt;
        playerY -= cosf(playerAngle) * playerSpeed * dt;
      }
    }

    // s press
    if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
    {
      playerX -= sinf(playerAngle) * playerSpeed * dt;
      playerY -= cosf(playerAngle) * playerSpeed * dt;
      if (mainMap.c_str()[(int)playerX * mapWidth * (int)playerY] == '#')
      {
        playerX += sinf(playerAngle) * playerSpeed * dt;
        playerY += cosf(playerAngle) * playerSpeed * dt;
      }
    }

    fps = 1.f / dt;
    });

  mainLoop->addToOnUpdate([]() {
    for (int x = 0; x < screenWidth; x++)
    {
      float ray_angle = (playerAngle - playerFOV / 2.f) + ((float)x / (float)screenWidth) * playerFOV;

      float step_size = .1f;
      float distance_to_wall = 0.f;

      bool hit_a_wall = false;
      bool hit_a_boundry = false;

      float eyeX = sinf(ray_angle);
      float eyeY = cosf(ray_angle);

      while (!hit_a_wall && distance_to_wall < playerViewDepth)
      {
        distance_to_wall += step_size;
        long long int testX = (int)(playerX + (double)eyeX * (double)distance_to_wall);
        long long int testY = (int)(playerY + (double)eyeY * (double)distance_to_wall);

        if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight)
        {
          hit_a_wall = true;
          distance_to_wall = playerViewDepth;
        }
        else if (mainMap[testY * mapWidth + testX] == '#')
        {
          hit_a_wall = true;
          std::vector<std::pair<float, float> > p;
          for (int tx = 0; tx < 2; tx++)
          {
            for (int ty = 0; ty < 2; ty++)
            {
              float vx = (float)testX + tx - playerX;
              float vy = (float)testY + ty - playerY;
              float d = sqrt(vx * vx + vy * vy);
              float dot = (eyeX * vx / d) + (eyeY * vy / d);
              p.push_back(std::make_pair(d, dot));
            }
          }
          std::sort(
            p.begin(),
            p.end(),
            [](const std::pair<float, float>& left, const std::pair<float, float>& right) { return left.first < right.first; }
          );
          float bound = .01f;
          if (acos(p.at(0).second) < bound) hit_a_boundry = true;
          if (acos(p.at(1).second) < bound) hit_a_boundry = true;
          if (acos(p.at(2).second) < bound) hit_a_boundry = true;
        }
      }

      int ceiling = (float)(screenHeight / 2.f) - screenHeight / ((float)distance_to_wall);
      int floor = screenHeight - ceiling;

      short shade = ' ';
      if (distance_to_wall <= playerViewDepth / 4.f) shade = 0x2588;  // very close
      else if (distance_to_wall < playerViewDepth / 3.f) shade = 0x2593;
      else if (distance_to_wall < playerViewDepth / 2.f) shade = 0x2592;
      else if (distance_to_wall < playerViewDepth) shade = 0x2591;
      else    shade = ' ';

      if (hit_a_boundry) shade = ' ';

      for (int i = 0; i < screenHeight; i++)
      {
        if (i <= ceiling)
        {
          consoleScreen[i * screenWidth + x] = ' ';
        }
        else if (i > ceiling && i <= floor)
        {
          consoleScreen[i * screenWidth + x] = shade;
        }
        else
        {
          float b = 1.f - (((float)i - screenHeight / 2.f) / ((float)screenHeight / 2.f));
          if (b < .25f) shade = '#';
          else if (b < .5f) shade = 'x';
          else if (b < .75f) shade = '.';
          else if (b < .9f) shade = '-';
          else shade = ' ';
          consoleScreen[i * screenWidth + x] = shade;
        }
      }
    }
    });

  mainLoop->addToOnPostUpdate([]() {
    //stats
    swprintf_s(consoleScreen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f, FPS=%3.2f", playerX, playerY, playerAngle, fps);
    //map
    for (int nx = 0; nx < mapWidth; nx++)
    {
      for (int ny = 0; ny < mapWidth; ny++)
      {
        consoleScreen[(ny + 1) * screenWidth + nx] = mainMap[ny * mapWidth + nx];
      }
    }
    consoleScreen[((int)playerX + 1) * screenWidth + (int)playerY] = 'P';

    // display frame
    consoleScreen[screenWidth * screenHeight - 1] = '\0';
    WriteConsoleOutputCharacter(consoleHandle, consoleScreen, screenWidth * screenHeight, { 0,0 }, &winBytesWritten);
    });


  return mainLoop->run();
}
````
