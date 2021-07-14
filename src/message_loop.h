//
// Created by guozhenxiong on 2021-07-14.
//

#ifndef TASKRUNNER_MESSAGELOOP_H
#define TASKRUNNER_MESSAGELOOP_H
namespace wtf {
class MessageLoop {
public:
    virtual void Run() = 0;
};
}
#endif //TASKRUNNER_MESSAGELOOP_H
