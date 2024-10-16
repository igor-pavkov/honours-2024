    int wm_SemInit(wm_Sem *s){
        s->lockCount = 0;
        pthread_mutex_init(&s->mutex, NULL);
        pthread_cond_init(&s->cond, NULL);
        return 0;
    }