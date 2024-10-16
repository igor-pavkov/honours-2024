char *am_urlencode(apr_pool_t *pool, const char *str)
{
    const char *ip;
    apr_size_t length;
    char *ret;
    char *op;
    int hi, low;
    /* Return NULL if str is NULL. */
    if(str == NULL) {
        return NULL;
    }


    /* Find the length of the output string. */
    length = 0;
    for(ip = str; *ip; ip++) {
        if(*ip >= 'a' && *ip <= 'z') {
            length++;
        } else if(*ip >= 'A' && *ip <= 'Z') {
            length++;
        } else if(*ip >= '0' && *ip <= '9') {
            length++;
        } else if(*ip == '_' || *ip == '.') {
            length++;
        } else {
            length += 3;
        }
    }

    /* Add space for null-terminator. */
    length++;

    /* Allocate memory for string. */
    ret = (char *)apr_palloc(pool, length);

    /* Encode string. */
    for(ip = str, op = ret; *ip; ip++, op++) {
        if(*ip >= 'a' && *ip <= 'z') {
            *op = *ip;
        } else if(*ip >= 'A' && *ip <= 'Z') {
            *op = *ip;
        } else if(*ip >= '0' && *ip <= '9') {
            *op = *ip;
        } else if(*ip == '_' || *ip == '.') {
            *op = *ip;
        } else {
            *op = '%';
            op++;

            hi = (*ip & 0xf0) >> 4;

            if(hi < 0xa) {
                *op = '0' + hi;
            } else {
                *op = 'A' + hi - 0xa;
            }
            op++;

            low = *ip & 0x0f;

            if(low < 0xa) {
                *op = '0' + low;
            } else {
                *op = 'A' + low - 0xa;
            }
        }
    }

    /* Make output string null-terminated. */
    *op = '\0';

    return ret;
}