static int connection_read_cq(connection *con, chunkqueue *cq, off_t max_bytes) {
    ssize_t len;
    size_t mem_len = 0;

    do {
        /* obtain chunk memory into which to read
         * fill previous chunk if it has a reasonable amount of space available
         * (use mem_len=0 to obtain large buffer at least half of chunk_buf_sz)
         */
        chunk *ckpt = cq->last;
        char * const mem = chunkqueue_get_memory(cq, &mem_len);
        if (mem_len > (size_t)max_bytes) mem_len = (size_t)max_bytes;

      #if defined(__WIN32)
        len = recv(con->fd, mem, mem_len, 0);
      #else
        len = read(con->fd, mem, mem_len);
      #endif

        chunkqueue_use_memory(cq, ckpt, len > 0 ? len : 0);

        if (len != (ssize_t)mem_len) {
            /* we got less then expected, wait for the next fd-event */
            con->is_readable = 0;

            if (len > 0) {
                con->bytes_read += len;
                return 0;
            }
            else if (0 == len) /* other end close connection -> KEEP-ALIVE */
                return -2;     /* (pipelining) */
            else
                return connection_read_cq_err(con);
        }

        con->bytes_read += len;
        max_bytes -= len;

        int frd;
        mem_len = (0 == fdevent_ioctl_fionread(con->fd, S_IFSOCK, &frd))
          ? (frd < max_bytes) ? (size_t)frd : (size_t)max_bytes
          : 0;
    } while (max_bytes);
    return 0;
}