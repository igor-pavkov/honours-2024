    int wm_SemFree(wm_Sem *s) {
        vSemaphoreDelete(*s);
        *s = NULL;
        return 0;
    }