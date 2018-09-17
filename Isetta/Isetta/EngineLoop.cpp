/*
 * Copyright (c) 2018 Isetta
 */
#include "EngineLoop.h"
#include "Audio/AudioSource.h"
#include "Core/Config/Config.h"
#include "Core/Debug/Logger.h"
#include "Core/ModuleManager.h"
#include "Core/Time/Clock.h"
#include "Graphics/LightNode.h"
#include "Graphics/ModelNode.h"
#include "Input/Input.h"
#include "Input/InputEnum.h"

namespace Isetta {
void EngineLoop::StartUp() {
  // config example
  Config::Instance().Read("config.cfg");

  // TODO(YIDI): Commented out by yidi
  // Sleep(3000);

  intervalTime = Config::Instance().fixedInterval.GetVal() / 1000.0;
  maxSimulationCount = Config::Instance().maxSimCount.GetVal();

  moduleManager.StartUp();

  GetGameClock();
  isGameRunning = true;

  Input::RegisterKeyPressCallback(KeyCode::ESCAPE,
                                  [&]() { isGameRunning = false; });

  // Game Init Part

  // Read scene from scene file
  ModelNode car{"test/Low-Poly-Racing-Car.scene.xml", Math::Vector3{0, -20, 0},
                Math::Vector3::zero, Math::Vector3::one};

  LightNode light{"materials/light.material.xml", Math::Vector3{0, 200, 600},
                  Math::Vector3::zero, Math::Vector3::one};

  // TODO(Chaojie) remove later into game logic

  // Register Input callback from player script
  U64 handleA, handleB, handleC;
  handleA = Input::RegisterKeyPressCallback(KeyCode::A, [&handleA]() {
    LOG_INFO(Debug::Channel::General, "A pressed");
    Input::UnregisterKeyPressCallback(KeyCode::A, handleA);
  });
  handleB = Input::RegisterKeyReleaseCallback(KeyCode::A, [&handleB]() {
    LOG_INFO(Debug::Channel::General, "A released");
    Input::UnregisterKeyReleaseCallback(KeyCode::A, handleB);
  });
  handleC = Input::RegisterMousePressCallback(
      MouseButtonCode::MOUSE_LEFT, [&handleC]() {
        LOG_INFO(Debug::Channel::General,
                 {"Left pressed at: " + Input::GetMousePosition().ToString()});
        Input::UnregisterMousePressCallback(MouseButtonCode::MOUSE_LEFT,
                                            handleC);
      });
}

void EngineLoop::Update() {
  GetGameClock().UpdateTime();

  // TODO(All) Add networking update

  // Client part
  accumulateTime += GetGameClock().GetDeltaTime();

  for (int i = 0; i < maxSimulationCount && accumulateTime > intervalTime;
       i++) {
    moduleManager.SimulationUpdate(intervalTime);
    LOG_INFO(Debug::Channel::General, std::to_string(accumulateTime));
    accumulateTime -= intervalTime;
  }

  // TODO(Chaojie) after scenegraph, save previous state for prediction
  moduleManager.RenderUpdate(0);
}

void EngineLoop::ShutDown() { moduleManager.ShutDown(); }

void EngineLoop::Run() {
  StartUp();
  while (isGameRunning) {
    Update();
  }
  ShutDown();
}

Clock& EngineLoop::GetGameClock() {
  static Clock gameTime{};
  return gameTime;
}
}  // namespace Isetta
