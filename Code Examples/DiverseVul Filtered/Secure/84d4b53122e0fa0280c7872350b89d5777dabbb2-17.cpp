    int wm_SemInit(wm_Sem *s) {
        *s = CreateSemaphore( NULL, 0, 1, NULL);
        return 0;
    }