// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "blocking_pool.h"

BlockingPool* BlockingPool::instance_ = NULL;

BlockingPool::BlockingPool()
    : blocking_pool_(new base::SequencedWorkerPool(2, "BlockingPool")) {
}

BlockingPool::~BlockingPool() {
  blocking_pool_->Shutdown(1000);
}

// static
void BlockingPool::CreateInstance() {
  if (!instance_)
    instance_ = new BlockingPool();
}

// static
BlockingPool* BlockingPool::GetInstance() {
  DCHECK(instance_) << "BlockingPool::CreateInstance must be called before "
      "getting the instance of BlockingPool.";
  return instance_;
}

// static
void BlockingPool::DeleteInstance() {
  delete instance_;
  instance_ = NULL;
}
      
// static
bool BlockingPool::PostBlockingPoolTask(
    const tracked_objects::Location& from_here,
    const base::Closure& task) {
  return BlockingPool::GetInstance()->blocking_pool_->PostWorkerTask(
      from_here, task);
}
// static
bool BlockingPool::PostBlockingPoolTaskAndReply(
    const tracked_objects::Location& from_here,
    const base::Closure& task,
    const base::Closure& reply) {
  return BlockingPool::GetInstance()->blocking_pool_->PostTaskAndReply(
      from_here, task, reply);
}
// static
bool BlockingPool::PostBlockingPoolSequencedTask(
    const std::string& sequence_token_name,
    const tracked_objects::Location& from_here,
    const base::Closure& task) {
  return BlockingPool::GetInstance()->blocking_pool_->PostNamedSequencedWorkerTask(
      sequence_token_name, from_here, task);
}
  
// static
base::SequencedWorkerPool* BlockingPool::GetBlockingPool() {
  return BlockingPool::GetInstance()->blocking_pool_.get();
}
