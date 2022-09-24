// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: BSD-3-Clause
// See: https://spdx.org/licenses/

#ifndef ABSTRACT_ACTOR_H_
#define ABSTRACT_ACTOR_H_

#include <functional>
#include <string>
#include <memory>
#include <utility>
#include <thread>
#include "shm.h"

namespace message_infrastructure {

enum ActorType {
  RuntimeActor = 0,
  RuntimeServiceActor = 1,
  ProcessModelActor = 2
};

enum ActorStatus {
  StatusError = -1,
  StatusRunning = 0,
  StatusStopped = 1,
  StatusPaused = 2
};

enum ActorCmd {
  CmdRun = 0,
  CmdStop = -1,
  CmdPause = -2
};

struct ActorCtrlStatus {
  ActorCmd cmd;
  ActorStatus status;
};

class AbstractActor {
 public:
  using ActorPtr = AbstractActor *;
  using TargetFn = std::function<void(ActorPtr)>;

  explicit AbstractActor(TargetFn target_fn);
  virtual int ForceStop() = 0;
  virtual int Wait() = 0;
  virtual int Create() = 0;
  void Control(const ActorCmd cmd);
  int GetStatus();
  void SetStatus(ActorStatus status);
  int GetPid() {
    return this->pid_;
  }

 protected:
  void Run();
  int pid_;

 private:
  SharedMemoryPtr ctl_shm_;
  std::atomic<int> actore_status_;
  std::shared_ptr<std::thread> handle_cmd_thread_ = nullptr;
  std::string actor_name_ = "actor";
  TargetFn target_fn_ = NULL;
  void InitStatus();
  void HandleCmd();
};

using SharedActorPtr = std::shared_ptr<AbstractActor>;

class PosixActor final : public AbstractActor {
 public:
  using AbstractActor::AbstractActor;
  int GetPid() {
    return this->pid_;
  }
  int Wait();
  int ForceStop();
  int Create();
};

using PosixActorPtr = PosixActor *;

}  // namespace message_infrastructure

#endif  // ABSTRACT_ACTOR_H_
