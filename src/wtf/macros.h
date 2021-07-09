//
// Created by admin on 2021/1/17.
//

#ifndef TEST_MACROS_H
#define TEST_MACROS_H

#define WTF_FWD(...) ::std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)

#if (USE_HTTP_BEAST__)
#define USE_HTTP_BEAST true
#endif


#define WTF_DISALLOW_COPY(TypeName) TypeName(const TypeName&) = delete

#define WTF_DISALLOW_ASSIGN(TypeName) \
  TypeName& operator=(const TypeName&) = delete

#define WTF_DISALLOW_MOVE(TypeName) \
  TypeName(TypeName&&) = delete;    \
  TypeName& operator=(TypeName&&) = delete

#define WTF_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;          \
  TypeName& operator=(const TypeName&) = delete

#define WTF_DISALLOW_COPY_ASSIGN_AND_MOVE(TypeName) \
  TypeName(const TypeName&) = delete;               \
  TypeName(TypeName&&) = delete;                    \
  TypeName& operator=(const TypeName&) = delete;    \
  TypeName& operator=(TypeName&&) = delete

#define WTF_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName() = delete;                               \
  WTF_DISALLOW_COPY_ASSIGN_AND_MOVE(TypeName)
#endif //TEST_MACROS_H
