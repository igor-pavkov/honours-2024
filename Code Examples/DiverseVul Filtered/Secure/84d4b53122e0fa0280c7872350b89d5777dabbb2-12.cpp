    int wm_SemLock(wm_Sem *s){
        pthread_mutex_lock(&s->mutex);
        while (s->lockCount > 0)
            pthread_cond_wait(&s->cond, &s->mutex);
        s->lockCount++;
        pthread_mutex_unlock(&s->mutex);
        return 0;
    }