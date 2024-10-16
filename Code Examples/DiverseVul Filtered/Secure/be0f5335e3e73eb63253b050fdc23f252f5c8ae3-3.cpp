AP_DECLARE(int) ap_rwrite(const void *buf, int nbyte, request_rec *r)
{
    if (r->connection->aborted)
        return -1;

    if (buffer_output(r, buf, nbyte) != APR_SUCCESS)
        return -1;

    return nbyte;
}