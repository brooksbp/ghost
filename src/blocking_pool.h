// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef BLOCKING_POOL_H_
#define BLOCKING_POOL_H_

#include "base/threading/sequenced_worker_pool.h"

class BlockingPool {
 public:
  BlockingPool();
  ~BlockingPool();

  static void CreateInstance();
  static BlockingPool* GetInstance();
  static void DeleteInstance();

  static bool PostBlockingPoolTask(
      const tracked_objects::Location& from_here, const base::Closure& task);
  static bool PostBlockingPoolTaskAndReply(
      const tracked_objects::Location& from_here, const base::Closure& task,
      const base::Closure& reply);
  static bool PostBlockingPoolSequencedTask(
      const std::string& sequence_token_name,
      const tracked_objects::Location& from_here, const base::Closure& task);
  
  static base::SequencedWorkerPool* GetBlockingPool();

 private:
  static BlockingPool* instance_;

  const scoped_refptr<base::SequencedWorkerPool> blocking_pool_;

  DISALLOW_COPY_AND_ASSIGN(BlockingPool);
};

#endif  // BLOCKING_POOL_H_
