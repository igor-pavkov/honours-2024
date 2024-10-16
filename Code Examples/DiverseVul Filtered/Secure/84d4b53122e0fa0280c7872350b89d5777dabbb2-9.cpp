    int wm_SemUnlock(wm_Sem *s) {
        xSemaphoreGive(*s);
        return 0;
    }