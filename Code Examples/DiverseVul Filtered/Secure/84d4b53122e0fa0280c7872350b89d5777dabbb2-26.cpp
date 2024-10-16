    int wm_SemFree(wm_Sem *s){
        if ((s == NULL) ||
            (*s == NULL))
            return MQTT_CODE_ERROR_BAD_ARG;
        dispatch_release(*s);
        *s = NULL;
        return 0;
    }