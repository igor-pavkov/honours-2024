AP_DECLARE(apr_status_t) ap_rgetline_core(char **s, apr_size_t n,
                                          apr_size_t *read, request_rec *r,
                                          int fold, apr_bucket_brigade *bb)
{
    apr_status_t rv;
    apr_bucket *e;
    apr_size_t bytes_handled = 0, current_alloc = 0;
    char *pos, *last_char = *s;
    int do_alloc = (*s == NULL), saw_eos = 0;

    /*
     * Initialize last_char as otherwise a random value will be compared
     * against APR_ASCII_LF at the end of the loop if bb only contains
     * zero-length buckets.
     */
    if (last_char)
        *last_char = '\0';

    for (;;) {
        apr_brigade_cleanup(bb);
        rv = ap_get_brigade(r->proto_input_filters, bb, AP_MODE_GETLINE,
                            APR_BLOCK_READ, 0);
        if (rv != APR_SUCCESS) {
            return rv;
        }

        /* Something horribly wrong happened.  Someone didn't block! */
        if (APR_BRIGADE_EMPTY(bb)) {
            return APR_EGENERAL;
        }

        for (e = APR_BRIGADE_FIRST(bb);
             e != APR_BRIGADE_SENTINEL(bb);
             e = APR_BUCKET_NEXT(e))
        {
            const char *str;
            apr_size_t len;

            /* If we see an EOS, don't bother doing anything more. */
            if (APR_BUCKET_IS_EOS(e)) {
                saw_eos = 1;
                break;
            }

            rv = apr_bucket_read(e, &str, &len, APR_BLOCK_READ);
            if (rv != APR_SUCCESS) {
                return rv;
            }

            if (len == 0) {
                /* no use attempting a zero-byte alloc (hurts when
                 * using --with-efence --enable-pool-debug) or
                 * doing any of the other logic either
                 */
                continue;
            }

            /* Would this overrun our buffer?  If so, we'll die. */
            if (n < bytes_handled + len) {
                *read = bytes_handled;
                if (*s) {
                    /* ensure this string is NUL terminated */
                    if (bytes_handled > 0) {
                        (*s)[bytes_handled-1] = '\0';
                    }
                    else {
                        (*s)[0] = '\0';
                    }
                }
                return APR_ENOSPC;
            }

            /* Do we have to handle the allocation ourselves? */
            if (do_alloc) {
                /* We'll assume the common case where one bucket is enough. */
                if (!*s) {
                    current_alloc = len;
                    *s = apr_palloc(r->pool, current_alloc);
                }
                else if (bytes_handled + len > current_alloc) {
                    /* Increase the buffer size */
                    apr_size_t new_size = current_alloc * 2;
                    char *new_buffer;

                    if (bytes_handled + len > new_size) {
                        new_size = (bytes_handled + len) * 2;
                    }

                    new_buffer = apr_palloc(r->pool, new_size);

                    /* Copy what we already had. */
                    memcpy(new_buffer, *s, bytes_handled);
                    current_alloc = new_size;
                    *s = new_buffer;
                }
            }

            /* Just copy the rest of the data to the end of the old buffer. */
            pos = *s + bytes_handled;
            memcpy(pos, str, len);
            last_char = pos + len - 1;

            /* We've now processed that new data - update accordingly. */
            bytes_handled += len;
        }

        /* If we got a full line of input, stop reading */
        if (last_char && (*last_char == APR_ASCII_LF)) {
            break;
        }
    }

    /* Now NUL-terminate the string at the end of the line;
     * if the last-but-one character is a CR, terminate there */
    if (last_char > *s && last_char[-1] == APR_ASCII_CR) {
        last_char--;
    }
    *last_char = '\0';
    bytes_handled = last_char - *s;

    /* If we're folding, we have more work to do.
     *
     * Note that if an EOS was seen, we know we can't have another line.
     */
    if (fold && bytes_handled && !saw_eos) {
        for (;;) {
            const char *str;
            apr_size_t len;
            char c;

            /* Clear the temp brigade for this filter read. */
            apr_brigade_cleanup(bb);

            /* We only care about the first byte. */
            rv = ap_get_brigade(r->proto_input_filters, bb, AP_MODE_SPECULATIVE,
                                APR_BLOCK_READ, 1);
            if (rv != APR_SUCCESS) {
                return rv;
            }

            if (APR_BRIGADE_EMPTY(bb)) {
                break;
            }

            e = APR_BRIGADE_FIRST(bb);

            /* If we see an EOS, don't bother doing anything more. */
            if (APR_BUCKET_IS_EOS(e)) {
                break;
            }

            rv = apr_bucket_read(e, &str, &len, APR_BLOCK_READ);
            if (rv != APR_SUCCESS) {
                apr_brigade_cleanup(bb);
                return rv;
            }

            /* Found one, so call ourselves again to get the next line.
             *
             * FIXME: If the folding line is completely blank, should we
             * stop folding?  Does that require also looking at the next
             * char?
             */
            /* When we call destroy, the buckets are deleted, so save that
             * one character we need.  This simplifies our execution paths
             * at the cost of one character read.
             */
            c = *str;
            if (c == APR_ASCII_BLANK || c == APR_ASCII_TAB) {
                /* Do we have enough space? We may be full now. */
                if (bytes_handled >= n) {
                    *read = n;
                    /* ensure this string is terminated */
                    (*s)[n-1] = '\0';
                    return APR_ENOSPC;
                }
                else {
                    apr_size_t next_size, next_len;
                    char *tmp;

                    /* If we're doing the allocations for them, we have to
                     * give ourselves a NULL and copy it on return.
                     */
                    if (do_alloc) {
                        tmp = NULL;
                    } else {
                        /* We're null terminated. */
                        tmp = last_char;
                    }

                    next_size = n - bytes_handled;

                    rv = ap_rgetline_core(&tmp, next_size,
                                          &next_len, r, 0, bb);
                    if (rv != APR_SUCCESS) {
                        return rv;
                    }

                    if (do_alloc && next_len > 0) {
                        char *new_buffer;
                        apr_size_t new_size = bytes_handled + next_len + 1;

                        /* we need to alloc an extra byte for a null */
                        new_buffer = apr_palloc(r->pool, new_size);

                        /* Copy what we already had. */
                        memcpy(new_buffer, *s, bytes_handled);

                        /* copy the new line, including the trailing null */
                        memcpy(new_buffer + bytes_handled, tmp, next_len + 1);
                        *s = new_buffer;
                    }

                    last_char += next_len;
                    bytes_handled += next_len;
                }
            }
            else { /* next character is not tab or space */
                break;
            }
        }
    }
    *read = bytes_handled;

    /* PR#43039: We shouldn't accept NULL bytes within the line */
    if (strlen(*s) < bytes_handled) {
        return APR_EINVAL;
    }

    return APR_SUCCESS;
}