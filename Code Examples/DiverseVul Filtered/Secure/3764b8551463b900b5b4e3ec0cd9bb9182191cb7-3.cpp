static int pull_client_data_chunked (struct conn_s *connptr) {
        char *buffer = 0;
        ssize_t len;
        long chunklen;

        while(1) {
                if (buffer) safefree(buffer);
                len = readline (connptr->client_fd, &buffer);

                if (len <= 0)
                        goto ERROR_EXIT;

                if (!connptr->error_variables) {
                        if (safe_write (connptr->server_fd, buffer, len) < 0)
                                goto ERROR_EXIT;
                }

                chunklen = strtol (buffer, (char**)0, 16);

                if (pull_client_data (connptr, chunklen+2, 0) < 0)
                        goto ERROR_EXIT;

                if(!chunklen) break;
        }

        safefree (buffer);
        return 0;

ERROR_EXIT:
        safefree (buffer);
        return -1;
}