// DemoConsoleGraphicsApp.cpp
// Credit for the original code in this file goes to: https://github.com/OneLoneCoder/CommandLineFPS
// I just adapted it to test out with my MainLoop static lib and may make various other changes as I go.
/*
DemoConsoleGraphicsApp
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
#include "../MainLoop/MainLoop.h"
#include <string>
#include <Windows.h>
#include <algorithm>

// note that you must set your console to 120 x 40 for this to look right in your console
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
