    int wm_SemLock(wm_Sem *s) {
        WaitForSingleObject(*s, 0);
        return 0;
    }