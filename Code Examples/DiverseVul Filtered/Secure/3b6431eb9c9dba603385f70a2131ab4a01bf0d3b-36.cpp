static const char *set_shmem_size(cmd_parms *cmd, void *config,
                                  const char *size_str)
{
    char *endptr;
    long  size, min;

    size = strtol(size_str, &endptr, 10);
    while (apr_isspace(*endptr)) endptr++;
    if (*endptr == '\0' || *endptr == 'b' || *endptr == 'B') {
        ;
    }
    else if (*endptr == 'k' || *endptr == 'K') {
        size *= 1024;
    }
    else if (*endptr == 'm' || *endptr == 'M') {
        size *= 1048576;
    }
    else {
        return apr_pstrcat(cmd->pool, "Invalid size in AuthDigestShmemSize: ",
                          size_str, NULL);
    }

    min = sizeof(*client_list) + sizeof(client_entry*) + sizeof(client_entry);
    if (size < min) {
        return apr_psprintf(cmd->pool, "size in AuthDigestShmemSize too small: "
                           "%ld < %ld", size, min);
    }

    shmem_size  = size;
    num_buckets = (size - sizeof(*client_list)) /
                  (sizeof(client_entry*) + HASH_DEPTH * sizeof(client_entry));
    if (num_buckets == 0) {
        num_buckets = 1;
    }
    ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, cmd->server, APLOGNO(01763)
                 "Set shmem-size: %" APR_SIZE_T_FMT ", num-buckets: %ld",
                 shmem_size, num_buckets);

    return NULL;
}