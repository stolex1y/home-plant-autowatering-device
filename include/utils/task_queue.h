#pragma once

#include <cinttypes>
#include <functional>
#include <queue>

namespace hpa::utils {

using Task = std::function<void()>;

class TaskQueue {
 public:
  explicit TaskQueue(size_t capacity = SIZE_MAX);

  bool Push(Task task);
  std::optional<Task> Pop();

 private:
  size_t capacity_;
  std::queue<Task> task_queue_;
};

}  // namespace hpa::utils
