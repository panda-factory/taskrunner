//
// Created by guozhenxiong on 2021-07-18.
//

#include "gtest/gtest.h"
#include "task_queues.h"

class TestWakeable : public wtf::Wakeable {
public:
    using WakeUpCall = std::function<void(const std::chrono::steady_clock::time_point& time_point)>;

    explicit TestWakeable(WakeUpCall call) : wake_up_call_(call) {}

    void WakeUp(const std::chrono::steady_clock::time_point& time_point) override { wake_up_call_(time_point); }

private:
    WakeUpCall wake_up_call_;
};

TEST(TaskQueues, StartsWithNoPendingTasks)
{
    auto task_queue = wtf::TaskQueues::GetInstance();
    auto queue_id = task_queue->CreateTaskQueue();
    ASSERT_FALSE(task_queue->HasPendingTasks(queue_id));
}

TEST(TaskQueues, RegisterOneTask)
{
    auto task_queue = wtf::TaskQueues::GetInstance();
    auto queue_id = task_queue->CreateTaskQueue();

    task_queue->RegisterTask(queue_id, [] {});
    ASSERT_TRUE(task_queue->HasPendingTasks(queue_id));
    ASSERT_TRUE(task_queue->GetNumPendingTasks(queue_id) == 1);
}

TEST(TaskQueues, RegisterOneTaskThenDispose)
{
    auto task_queue = wtf::TaskQueues::GetInstance();
    auto queue_id = task_queue->CreateTaskQueue();

    task_queue->RegisterTask(queue_id, [] {});
    ASSERT_TRUE(task_queue->HasPendingTasks(queue_id));
    ASSERT_TRUE(task_queue->GetNumPendingTasks(queue_id) == 1);

    task_queue->Dispose(queue_id);
    ASSERT_FALSE(task_queue->HasPendingTasks(queue_id));
    ASSERT_TRUE(task_queue->GetNumPendingTasks(queue_id) == 0);
}

TEST(TaskQueues, RegisterOneTaskThenDisposeTasks)
{
    auto task_queue = wtf::TaskQueues::GetInstance();
    auto queue_id = task_queue->CreateTaskQueue();

    task_queue->RegisterTask(queue_id, [] {});
    ASSERT_TRUE(task_queue->HasPendingTasks(queue_id));
    ASSERT_TRUE(task_queue->GetNumPendingTasks(queue_id) == 1);

    task_queue->DisposeTasks(queue_id);
    ASSERT_FALSE(task_queue->HasPendingTasks(queue_id));
    ASSERT_TRUE(task_queue->GetNumPendingTasks(queue_id) == 0);
}

TEST(TaskQueues, RegisterTwoTasksAndCount) {
    auto task_queue = wtf::TaskQueues::GetInstance();
    auto queue_id = task_queue->CreateTaskQueue();
    task_queue->RegisterTask(queue_id, [] {});
    task_queue->RegisterTask(queue_id, [] {});
    ASSERT_TRUE(task_queue->HasPendingTasks(queue_id));
    ASSERT_TRUE(task_queue->GetNumPendingTasks(queue_id) == 2);
}

TEST(TaskQueues, PreserveTaskOrdering) {
    auto task_queue = wtf::TaskQueues::GetInstance();
    auto queue_id = task_queue->CreateTaskQueue();
    int test_val = 0;

    // order: 0
    task_queue->RegisterTask(
            queue_id, [&test_val]() { test_val = 1; });

    // order: 1
    task_queue->RegisterTask(
            queue_id, [&test_val]() { test_val = 2; });

    int expected_value = 1;
    for (;;) {
        std::function<void()> invoker = task_queue->GetNextTask(queue_id);
        if (!invoker) {
            break;
        }
        invoker();
        ASSERT_TRUE(test_val == expected_value);
        expected_value++;
    }
}

void TestNotifyObservers(wtf::TaskQueueId queue_id) {
    auto task_queue = wtf::TaskQueues::GetInstance();
    std::vector<std::function<void()>> observers =
            task_queue->GetObserversToNotify(queue_id);
    for (const auto& observer : observers) {
        observer();
    }
}

TEST(TaskQueues, AddRemoveNotifyObservers) {
    auto task_queue = wtf::TaskQueues::GetInstance();
    auto queue_id = task_queue->CreateTaskQueue();

    int test_val = 0;
    intptr_t key = 123;

    task_queue->AddTaskObserver(queue_id, key, [&test_val]() { test_val = 1; });
    TestNotifyObservers(queue_id);
    ASSERT_TRUE(test_val == 1);

    test_val = 0;
    task_queue->RemoveTaskObserver(queue_id, key);
    TestNotifyObservers(queue_id);
    ASSERT_TRUE(test_val == 0);
}

TEST(TaskQueues, WakeUpIndependentOfTime) {
    auto task_queue = wtf::TaskQueues::GetInstance();
    auto queue_id = task_queue->CreateTaskQueue();

    int num_wakes = 0;
    task_queue->SetWakeable(
            queue_id, new TestWakeable(
                    [&num_wakes](const std::chrono::steady_clock::time_point& wake_time) { ++num_wakes; }));

    task_queue->RegisterTask(
            queue_id, []() {});
    task_queue->RegisterTask(
            queue_id, []() {});

    ASSERT_TRUE(num_wakes == 2);
}
