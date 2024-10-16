    int wm_SemUnlock(wm_Sem *s) {
        ReleaseSemaphore(*s, 1, NULL);
        return 0;
    }