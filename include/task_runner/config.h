//
// Created by admin on 2021-07-13.
//

#ifndef TASKRUNNER_CONFIG_H
#define TASKRUNNER_CONFIG_H

#if defined(STATIC_LIB)
#define WTF_DLL
#else
#if !defined(USE_DLL)
#define WTF_DLL     __declspec(dllexport)
#else         /* use a DLL library */
#define WTF_DLL     __declspec(dllimport)
#endif
#endif

#endif //TASKRUNNER_CONFIG_H
