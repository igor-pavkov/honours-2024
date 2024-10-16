static void randomsleep(unsigned int t) {
    usleep2((unsigned long) (zrand() % PASSWD_FAILURE_DELAY));
    usleep2(t * PASSWD_FAILURE_DELAY);
}