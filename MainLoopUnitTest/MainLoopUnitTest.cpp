/*
MainLoopUnitTest
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

#include "pch.h"
#include "CppUnitTest.h"
#include "../MainLoop/MainLoop.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MainLoopUnitTest
{
TEST_CLASS(MainLoopUnitTest)
{
public:

  TEST_METHOD(TestBaseInitAndRun)
  {
    Assert::AreEqual(MainLoop::isLive(), false);
    Assert::AreEqual(MainLoop::init()->run(), 0);
    Assert::AreEqual(MainLoop::isLive(), false);
  }

  TEST_METHOD(TestQuitCon)
  {
    static bool keepRunning = false;
    auto ml = MainLoop::init();
    ml->setRunCon([]() { return keepRunning; });
    ml->addToOnBegin([]() { keepRunning = true; });
    ml->addToOnUpdate([]() { keepRunning = false; });
    Assert::AreEqual(ml->run(), 0);
    Assert::AreEqual(MainLoop::isLive(), false);
  }

  TEST_METHOD(TestLiveCheck)
  {
    static bool keepRunning = false;
    auto ml = MainLoop::init();
    Assert::AreEqual(MainLoop::isLive(), true);
    ml->setRunCon([]() { return keepRunning; });
    ml->addToOnBegin([]() { keepRunning = true; });
    ml->addToOnUpdate([]() { keepRunning = false; });
    int retval = ml->run();
    Assert::AreEqual(retval, 0);
    Assert::AreEqual(MainLoop::isLive(), false);
  }

  TEST_METHOD(Test100TickCount)
  {
    static bool keepRunning = false;
    auto ml = MainLoop::init();
    ml->setRunCon([]() { return keepRunning; });
    ml->addToOnBegin([]() { keepRunning = true; });
    static int ticks = 0;
    ml->addToOnUpdate([]() { ticks++; if (ticks > 99) keepRunning = false; });
    Assert::AreEqual(ml->run(), 0);
    Assert::AreEqual(ticks, 100);
  }

  TEST_METHOD(Test1000TickCount)
  {
    static bool keepRunning = false;
    auto ml = MainLoop::init();
    ml->setRunCon([]() { return keepRunning; });
    ml->addToOnBegin([]() { keepRunning = true; });
    static int ticks = 0;
    ml->addToOnUpdate([]() { ticks++; if (ticks > 999) keepRunning = false; });
    Assert::AreEqual(ml->run(), 0);
    Assert::AreEqual(ticks, 1000);
  }

  TEST_METHOD(Test2000TickCount)
  {
    static bool keepRunning = false;
    auto ml = MainLoop::init();
    ml->setRunCon([]() { return keepRunning; });
    ml->addToOnBegin([]() { keepRunning = true; });
    static int ticks = 0;
    ml->addToOnUpdate([]() { ticks++; if (ticks > 1999) keepRunning = false; });
    Assert::AreEqual(ml->run(), 0);
    Assert::AreEqual(ticks, 2000);
  }

  TEST_METHOD(Test3000TickCount)
  {
    static bool keepRunning = false;
    auto ml = MainLoop::init();
    ml->setRunCon([]() { return keepRunning; });
    ml->addToOnBegin([]() { keepRunning = true; });
    static int ticks = 0;
    ml->addToOnUpdate([]() { ticks++; if (ticks > 2999) keepRunning = false; });
    Assert::AreEqual(ml->run(), 0);
    Assert::AreEqual(ticks, 3000);
  }
  TEST_METHOD(TestDeltaTime)
  {
    static bool keepRunning = false;
    auto ml = MainLoop::init();
    ml->setRunCon([]() { return keepRunning; });
    ml->addToOnBegin([]() { keepRunning = true; });
  }

};
}
