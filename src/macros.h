//
// Created by guozhenxiong on 2021-07-13.
//

#ifndef TASKRUNNER_MACROS_H
#define TASKRUNNER_MACROS_H


#define WTF_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;          \
  TypeName& operator=(const TypeName&) = delete

#define WTF_DISALLOW_COPY_ASSIGN_AND_MOVE(TypeName) \
  TypeName(const TypeName&) = delete;               \
  TypeName(TypeName&&) = delete;                    \
  TypeName& operator=(const TypeName&) = delete;    \
  TypeName& operator=(TypeName&&) = delete

#endif //TASKRUNNER_MACROS_H
