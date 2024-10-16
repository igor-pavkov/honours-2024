    int wm_SemLock(wm_Sem *s){
        dispatch_semaphore_wait(*s, DISPATCH_TIME_FOREVER);
        return 0;
    }