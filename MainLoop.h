/*
MainLoop.h
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

/* Usage:
  1. Populate the run functions with lambdas i.e. MainLoop::Get()->AddToOnUpdate([](){std::cout << "update!\n"; };
  2. Start MainLoop::Run()
  3. Watch your simulation go.
*/

#pragma once
#include <functional>
#include <memory>
class MainLoop
{
public:
  static std::shared_ptr<MainLoop> Init();
  static std::shared_ptr<MainLoop> Get();
  bool IsLive();
  void SetRunCondition(bool(*function)());
  void AddToOnBegin(void(*function)());
  void AddToOnUpdate(void(*function)(float));
  void AddToOnUpdate(void(*function)());
  void AddToOnPostUpdate(void(*function)());
  void AddToOnTeardown(void(*function)());
  int Run();
private:
  bool p_CheckRunCondition();
  void p_Begin();
  void p_Update();
  void p_Render();
  void p_Teardown();
};

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
#include <vector>
#include <chrono>

static std::shared_ptr<MainLoop> __MainLoop__ = NULL;

std::shared_ptr<MainLoop> MainLoop::Init()
{
	if (__MainLoop__ == NULL)
	{
		__MainLoop__ = std::make_shared<MainLoop>();
	}
	return __MainLoop__;
}

std::shared_ptr<MainLoop> MainLoop::Get()
{
	return MainLoop::Init();
}

bool MainLoop::IsLive()
{
	return __MainLoop__ != NULL;
}

static std::function<bool()>                    onContinueCheck;
static std::vector<std::function<void()> >      onBegin;
static std::vector<std::function<void(float)> > onDeltaUpdate;
static std::vector<std::function<void()> >      onUpdate;
static std::vector<std::function<void()> >      onRender;
static std::vector<std::function<void()> >      onTeardown;

void MainLoop::SetRunCondition(bool(*function)())
{
	onContinueCheck = function;
}

void MainLoop::AddToOnBegin(void(*function)())
{
	onBegin.push_back(function);
}

void MainLoop::AddToOnUpdate(void(*function)(float))
{
	onDeltaUpdate.push_back(function);
}

void MainLoop::AddToOnUpdate(void(*function)())
{
	onUpdate.push_back(function);
}

void MainLoop::AddToOnPostUpdate(void(*function)())
{
	onRender.push_back(function);
}

void MainLoop::AddToOnTeardown(void(*function)())
{
	onTeardown.push_back(function);
}

int MainLoop::Run()
{
	if (__MainLoop__ == NULL)
		MainLoop::Init();
	p_Begin();
	while (p_CheckRunCondition())
	{
		p_Update();
		p_Render();
	}
	p_Teardown();
	return 0;
}

bool MainLoop::p_CheckRunCondition()
{
	if (onContinueCheck == NULL)
		return false;
	else
		return onContinueCheck();
}

void MainLoop::p_Begin()
{
	for (const auto& oB : onBegin)
	{
		oB();
	}
}


void MainLoop::p_Update()
{

	for (const auto& oU : onUpdate)
	{
		oU();
	}

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


}

void MainLoop::p_Render()
{
	for (const auto& oR : onRender)
	{
		oR();
	}
}

void MainLoop::p_Teardown()
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

	__MainLoop__ = NULL;
}

