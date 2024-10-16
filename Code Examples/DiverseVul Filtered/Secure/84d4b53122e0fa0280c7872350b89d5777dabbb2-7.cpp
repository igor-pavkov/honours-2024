    int wm_SemUnlock(wm_Sem *s){
        dispatch_semaphore_signal(*s);
        return 0;
    }