#include "task_queue.h"

namespace hpa::tasks {

TaskQueue::TaskQueue(size_t capacity) : capacity_(capacity) {
}

bool TaskQueue::Push(Task task) {
  if (task_queue_.size() >= capacity_) {
    return false;
  }
  task_queue_.emplace(std::move(task));
  return true;
}

std::optional<Task> TaskQueue::Pop() {
  if (task_queue_.empty()) {
    return std::nullopt;
  }
  auto task = task_queue_.front();
  task_queue_.pop();
  return task;
}

}  // namespace hpa::tasks
