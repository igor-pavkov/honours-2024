bool conn_add_to_freelist(conn *c) {
    bool ret = true;
    pthread_mutex_lock(&conn_lock);
    if (freecurr < freetotal) {
        freeconns[freecurr++] = c;
        ret = false;
    } else {
        /* try to enlarge free connections array */
        size_t newsize = freetotal * 2;
        conn **new_freeconns = realloc(freeconns, sizeof(conn *) * newsize);
        if (new_freeconns) {
            freetotal = newsize;
            freeconns = new_freeconns;
            freeconns[freecurr++] = c;
            ret = false;
        }
    }
    pthread_mutex_unlock(&conn_lock);
    return ret;
}