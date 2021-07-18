//
// Created by guozhenxiong on 2021-07-16.
//

#include "message_loop_impl.h"
#include "message_loop.h"
#include "logging/logging.h"

namespace wtf {


MessageLoopImpl::MessageLoopImpl() :
        task_queues_(TaskQueues::GetInstance()),
        queue_id_(task_queues_->CreateTaskQueue()),
        terminated_(false)
{
    task_queues_->SetWakeable(queue_id_, this);
}

MessageLoopImpl::MessageLoopImpl(wtf::Wakeable *wakeable)
{
    task_queues_->SetWakeable(queue_id_, wakeable);
}

MessageLoopImpl::~MessageLoopImpl()
{

}

void MessageLoopImpl::AddTaskObserver(intptr_t key,
                                      const std::function<void ()> &callback)
{
    WTF_DCHECK(callback != nullptr);
    WTF_DCHECK(MessageLoop::GetCurrent().GetLoopImpl() == this)
    << "Message loop task observer must be added on the same thread as the "
       "loop.";
    if (callback != nullptr) {
        task_queues_->AddTaskObserver(queue_id_, key, callback);
    } else {
        WTF_LOG(ERROR) << "Tried to add a null TaskObserver.";
    }
}

void MessageLoopImpl::FlushTasks(FlushType type)
{
    std::function<void ()> invoker;
    do {
        invoker = task_queues_->GetNextTask(queue_id_);
        if (!invoker) {
            break;
        }
        invoker();
        std::vector<std::function<void ()>> observers =
                task_queues_->GetObserversToNotify(queue_id_);
        for (const auto &observer : observers) {
            observer();
        }
        if (type == FlushType::kSingle) {
            break;
        }
    } while (invoker);
}

void MessageLoopImpl::RemoveTaskObserver(intptr_t key) {
    WTF_DCHECK(MessageLoop::GetCurrent().GetLoopImpl() == this)
    << "Message loop task observer must be removed from the same thread as "
       "the loop.";
    task_queues_->RemoveTaskObserver(queue_id_, key);
}

void MessageLoopImpl::PostTask(const std::function<void ()>& task,
                               const std::chrono::steady_clock::time_point&)
{
    WTF_DCHECK(task != nullptr);
    if (terminated_) {
        // If the message loop has already been terminated, PostTask should destruct
        // |task| synchronously within this function.
        return;
    }
    task_queues_->RegisterTask(queue_id_, task);
}

void MessageLoopImpl::DoRun()
{
    if (terminated_) {
        return;
    }

    Run();
    terminated_ = true;
}

void MessageLoopImpl::Run()
{
    while (!terminated_) {
        std::unique_lock lock(task_mutex_);
        task_condition_.wait(lock);

        FlushTasks(FlushType::kAll);
    }
}

void MessageLoopImpl::Terminate()
{
}

void MessageLoopImpl::DoTerminate()
{
    terminated_ = true;
    Terminate();
}


void MessageLoopImpl::WakeUp(const std::chrono::steady_clock::time_point& time_point)
{
    task_condition_.notify_one();
}

} // namespace wtf