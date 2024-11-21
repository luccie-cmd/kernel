#include <kernel/task/task.h>
#include <common/dbg/dbg.h>
#define MODULE "Task manager"

namespace task{
    pid_t currentPID;
    bool initialized = false;
    void initialize(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "Initializing...\n");
        initialized = true;
        currentPID = KERNEL_PID;
        dbg::printm(MODULE, "Initialized\n");
        dbg::popTrace();
    }
    bool isInitialized(){
        return initialized;
    }
    pid_t getCurrentPID(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize();
        }
        dbg::popTrace();
        return currentPID;
    }
};