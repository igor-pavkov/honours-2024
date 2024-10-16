    int wm_SemFree(wm_Sem *s) {
        CloseHandle(*s);
        *s = NULL;
        return 0;
    }