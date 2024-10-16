    int wm_SemInit(wm_Sem *s) {
        *s = xSemaphoreCreateBinary();
        xSemaphoreGive(*s);
        return 0;
    }