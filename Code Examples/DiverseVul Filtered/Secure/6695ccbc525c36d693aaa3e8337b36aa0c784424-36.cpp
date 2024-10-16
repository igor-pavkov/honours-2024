static void conn_init(void) {
    freetotal = 200;
    freecurr = 0;
    if ((freeconns = calloc(freetotal, sizeof(conn *))) == NULL) {
        fprintf(stderr, "Failed to allocate connection structures\n");
    }
    return;
}