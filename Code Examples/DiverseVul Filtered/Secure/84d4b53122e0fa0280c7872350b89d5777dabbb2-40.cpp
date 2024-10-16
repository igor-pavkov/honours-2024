    int wm_SemLock(wm_Sem *s) {
        xSemaphoreTake(*s, portMAX_DELAY);
        return 0;
    }