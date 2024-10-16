int processEventsWhileBlocked(void) {
    int iterations = 4; /* See the function top-comment. */
    int count = 0;
    while (iterations--) {
        int events = 0;
        events += aeProcessEvents(server.el, AE_FILE_EVENTS|AE_DONT_WAIT);
        events += handleClientsWithPendingWrites();
        if (!events) break;
        count += events;
    }
    return count;
}