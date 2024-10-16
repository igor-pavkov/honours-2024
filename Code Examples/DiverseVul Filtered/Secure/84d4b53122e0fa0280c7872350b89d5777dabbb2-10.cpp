    int wm_SemFree(wm_Sem *s){
        pthread_mutex_destroy(&s->mutex);
        pthread_cond_destroy(&s->cond);
        return 0;
    }