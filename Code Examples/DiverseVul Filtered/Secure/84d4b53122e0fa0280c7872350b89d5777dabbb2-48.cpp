    int wm_SemInit(wm_Sem *s){
        /* dispatch_release() fails hard, with Trace/BPT trap signal, if the
         * sem's internal count is less than the value passed in with
         * dispatch_semaphore_create().  work around this by initing
         * with 0, then incrementing it afterwards.
         */
        *s = dispatch_semaphore_create(0);
        if (*s == NULL)
            return MQTT_CODE_ERROR_MEMORY;
        if (dispatch_semaphore_signal(*s) < 0) {
            dispatch_release(*s);
            return MQTT_CODE_ERROR_SYSTEM;
        }

        return 0;
    }