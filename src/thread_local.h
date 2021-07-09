//
// Created by admin on 2021-07-02.
//

#ifndef TEST_THREAD_LOCAL_H
#define TEST_THREAD_LOCAL_H

#include <memory>

#include "wtf/macros.h"
namespace wtf {

#define WTF_THREAD_LOCAL static thread_local

#ifndef WTF_THREAD_LOCAL

#error Thread local storage unavailable on the platform.

#endif  // WTF_THREAD_LOCAL

} // namespace wtf


#endif //TEST_THREAD_LOCAL_H
