//
// Created by guozhenxiong on 2021-07-02.
//

#include "delayed_message_loop_impl.h"
#include "delayed_message_loop.h"
#include "platform/win/delayed_message_loop_win.h"
#include "logging/logging.h"

namespace wtf {

std::unique_ptr<DelayedMessageLoopImpl> DelayedMessageLoopImpl::Create()
{
#if OS_WIN
    return std::make_unique<DelayedMessageLoopWin>();
#else
    return nullptr;
#endif
}

DelayedMessageLoopImpl::DelayedMessageLoopImpl()
        : task_queue_(MessageLoopTaskQueues::GetInstance()),
          queue_id_(task_queue_->CreateTaskQueue()),
          terminated_(false)
{
    task_queue_->SetWakeable(queue_id_, this);
}

DelayedMessageLoopImpl::~DelayedMessageLoopImpl()
{
    task_queue_->Dispose(queue_id_);
}

void DelayedMessageLoopImpl::AddTaskObserver(intptr_t key,
                                      const std::function<void ()> &callback)
{
    WTF_DCHECK(callback != nullptr);
    WTF_DCHECK(DelayedMessageLoop::GetCurrent().GetLoopImpl() == this)
    << "Message loop task observer must be added on the same thread as the "
       "loop.";
    if (callback != nullptr) {
        task_queue_->AddTaskObserver(queue_id_, key, callback);
    } else {
        WTF_LOG(ERROR) << "Tried to add a null TaskObserver.";
    }
}

void DelayedMessageLoopImpl::RemoveTaskObserver(intptr_t key) {
    WTF_DCHECK(DelayedMessageLoop::GetCurrent().GetLoopImpl() == this)
            << "Message loop task observer must be removed from the same thread as "
               "the loop.";
    task_queue_->RemoveTaskObserver(queue_id_, key);
}

void DelayedMessageLoopImpl::DoRun()
{
    if (terminated_) {
        return;
    }

    Run();
    terminated_ = true;

    RunExpiredTasksNow();

    task_queue_->DisposeTasks(queue_id_);
}

void DelayedMessageLoopImpl::DoTerminate()
{
    terminated_ = true;
    Terminate();
}

void DelayedMessageLoopImpl::FlushTasks(FlushType type)
{

    const auto now = std::chrono::steady_clock::now();
    std::function<void ()> invocation;
    do {
        invocation = task_queue_->GetNextTaskToRun(queue_id_, now);
        if (!invocation) {
            break;
        }
        invocation();
        std::vector<std::function<void ()>> observers =
                task_queue_->GetObserversToNotify(queue_id_);
        for (const auto &observer : observers) {
            observer();
        }
        if (type == FlushType::Single) {
            break;
        }
    } while (invocation);
}

void DelayedMessageLoopImpl::PostTask(const std::function<void ()> &task,
                               const std::chrono::steady_clock::time_point& target_time)
{
    WTF_DCHECK(task != nullptr);
    WTF_DCHECK(task != nullptr);
    if (terminated_) {
        // If the message loop has already been terminated, PostTask should destruct
        // |task| synchronously within this function.
        return;
    }
    task_queue_->RegisterTask(queue_id_, task, target_time);
}

void DelayedMessageLoopImpl::RunExpiredTasksNow()
{
    FlushTasks(FlushType::All);
}

TaskQueueId DelayedMessageLoopImpl::GetTaskQueueId() const
{
    return queue_id_;
}

} // namespace wtf