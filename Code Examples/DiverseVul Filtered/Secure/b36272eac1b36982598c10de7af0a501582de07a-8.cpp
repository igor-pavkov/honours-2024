void ssh_reseed(void){
#ifndef _WIN32
    struct timeval tv;
    gettimeofday(&tv, NULL);
    RAND_add(&tv, sizeof(tv), 0.0);
#endif
}