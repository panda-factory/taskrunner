//
// Created by guozhenxiong on 2021-07-02.
//

#ifndef TASKRUNNER_THREAD_LOCAL_H
#define TASKRUNNER_THREAD_LOCAL_H

#include <memory>

namespace wtf {

#define WTF_THREAD_LOCAL static thread_local

#ifndef WTF_THREAD_LOCAL

#error Thread local storage unavailable on the platform.

#endif  // WTF_THREAD_LOCAL

} // namespace wtf


#endif //TASKRUNNER_THREAD_LOCAL_H
