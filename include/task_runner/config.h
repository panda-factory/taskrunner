//
// Created by guozhenxiong on 2021-07-13.
//

#ifndef TASKRUNNER_CONFIG_H
#define TASKRUNNER_CONFIG_H

#if BUILDING_SHARED
#define WTF_DLL     __declspec(dllexport)
#elif USING_SHARED         /* use a DLL library */
#define WTF_DLL     __declspec(dllimport)
#else
#define WTF_DLL
#endif

#endif //TASKRUNNER_CONFIG_H
