void doreply(void)
{
    struct reply *scannedentry;
    struct reply *nextentry;

    if ((scannedentry = firstreply) == NULL) {
        return;
    }
    do {
        nextentry = scannedentry->next;
#ifdef WITH_TLS
        if (tls_cnx != NULL) {
            char buf[MAX_SERVER_REPLY_LEN];

            snprintf(buf, sizeof buf, "%3d%c%s\r\n", replycode,
                     nextentry == NULL ? ' ' : '-', scannedentry->line);
            SSL_write(tls_cnx, buf, strlen(buf));
        } else
#endif
        {
            client_printf("%3d%c%s\r\n", replycode,
                          nextentry == NULL ? ' ' : '-',
                          scannedentry->line);
        }
        if (logging > 1) {
            logfile(LOG_DEBUG, "%3d%c%s", replycode,
                    nextentry == NULL ? ' ' : '-', scannedentry->line);
        }
    } while ((scannedentry = nextentry) != NULL);
    client_fflush();
    scannedentry = firstreply;
    do {
        nextentry = scannedentry->next;
        free(scannedentry);
    } while ((scannedentry = nextentry) != NULL);
    firstreply = lastreply = NULL;
}