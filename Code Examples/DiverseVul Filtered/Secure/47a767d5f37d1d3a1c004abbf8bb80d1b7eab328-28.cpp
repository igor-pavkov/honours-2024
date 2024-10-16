char *am_generate_id(request_rec *r)
{
    int rc;
    char *ret;
    int rand_data_len;
    unsigned char *rand_data;
    int i;
    unsigned char b;
    int hi, low;

    ret = (char *)apr_palloc(r->pool, AM_ID_LENGTH + 1);

    /* We need to round the length of the random data _up_, in case the
     * length of the session id isn't even.
     */
    rand_data_len = (AM_ID_LENGTH + 1) / 2;

    /* Fill the last rand_data_len bytes of the string with
     * random bytes. This allows us to overwrite from the beginning of
     * the string.
     */
    rand_data = (unsigned char *)&ret[AM_ID_LENGTH - rand_data_len];

    /* Generate random numbers. */
    rc = am_generate_random_bytes(r, rand_data, rand_data_len);
    if(rc != OK) {
        return NULL;
    }

    /* Convert the random bytes to hexadecimal. Note that we will write
     * AM_ID_LENGTH+1 characters if we have a non-even length of the
     * session id. This is OK - we will simply overwrite the last character
     * with the null-terminator afterwards.
     */
    for(i = 0; i < AM_ID_LENGTH; i += 2) {
        b = rand_data[i / 2];
        hi = (b >> 4) & 0xf;
        low = b & 0xf;

        if(hi >= 0xa) {
            ret[i] = 'a' + hi - 0xa;
        } else {
            ret[i] = '0' + hi;
        }

        if(low >= 0xa) {
            ret[i+1] = 'a' + low - 0xa;
        } else {
            ret[i+1] = '0' + low;
        }
    }

    /* Add null-terminator- */
    ret[AM_ID_LENGTH] = '\0';

    return ret;
}