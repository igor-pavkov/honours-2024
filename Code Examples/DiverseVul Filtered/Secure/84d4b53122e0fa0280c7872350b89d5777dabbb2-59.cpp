    int wm_SemUnlock(wm_Sem *s){
        pthread_mutex_lock(&s->mutex);
        s->lockCount--;
        pthread_cond_signal(&s->cond);
        pthread_mutex_unlock(&s->mutex);
        return 0;
    }