/*
MainLoop.cpp
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
#include "framework.h"
#include "pch.h"
#include "MainLoop.h"
#include <memory>
#include <chrono>

static std::shared_ptr<MainLoop> smLoop = NULL;

std::shared_ptr<MainLoop> MainLoop::init()
{
  if (smLoop == NULL)
  {
    smLoop = std::make_shared<MainLoop>();
  }
  return smLoop;
}

std::shared_ptr<MainLoop> MainLoop::get()
{
  return MainLoop::init();
}

bool MainLoop::isLive()
{
  return smLoop != NULL;
}

static std::function<bool()>                    onContinueCheck;
static std::vector<std::function<void()> >      onBegin;
static std::vector<std::function<void(float)> > onDeltaUpdate;
static std::vector<std::function<void()> >      onUpdate;
static std::vector<std::function<void()> >      onRender;
static std::vector<std::function<void()> >      onTeardown;

void MainLoop::setRunCon(bool(*function)())
{
  onContinueCheck = function;
}

void MainLoop::addToOnBegin(void(*function)())
{
  onBegin.push_back(function);
}

void MainLoop::addToOnUpdate(void(*function)(float))
{
  onDeltaUpdate.push_back(function);
}

void MainLoop::addToOnUpdate(void(*function)())
{
  onUpdate.push_back(function);
}

void MainLoop::addToOnPostUpdate(void(*function)())
{
  onRender.push_back(function);
}

void MainLoop::addToOnTeardown(void(*function)())
{
  onTeardown.push_back(function);
}

int MainLoop::run()
{
  if (smLoop == NULL)
    MainLoop::init();
  u_begin();
  while (u_check_run_con())
  {
    u_update();
    u_render();
  }
  u_teardown();
  return 0;
}

bool MainLoop::u_check_run_con()
{
  if (onContinueCheck == NULL)
    return false;
  else
    return onContinueCheck();
}

void MainLoop::u_begin()
{
  for (const auto& oB : onBegin)
  {
    oB();
  }
}


void MainLoop::u_update()
{
  static std::chrono::system_clock::time_point currTime;
  static std::chrono::system_clock::time_point lastTime = std::chrono::system_clock::now();
  static std::chrono::duration<float> deltaTime;

  currTime = std::chrono::system_clock::now();
  deltaTime = currTime - lastTime;
  lastTime = currTime;

  float elapsedTime = deltaTime.count();

  for (const auto& oDU : onDeltaUpdate)
  {
    oDU(elapsedTime);
  }

  for (const auto& oU : onUpdate)
  {
    oU();
  }
}

void MainLoop::u_render()
{
  for (const auto& oR : onRender)
  {
    oR();
  }
}

void MainLoop::u_teardown()
{
  for (const auto& oT : onTeardown)
  {
    oT();
  }

  onBegin.clear();
  onDeltaUpdate.clear();
  onUpdate.clear();
  onRender.clear();
  onTeardown.clear();

  smLoop = NULL;
}
