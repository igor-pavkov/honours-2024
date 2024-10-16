static void sha1_update(SHA_CTX *c, const void *data, size_t len)
{
    const unsigned char *ptr = data;
    size_t res;

    if ((res = c->num)) {
        res = SHA_CBLOCK - res;
        if (len < res)
            res = len;
        SHA1_Update(c, ptr, res);
        ptr += res;
        len -= res;
    }

    res = len % SHA_CBLOCK;
    len -= res;

    if (len) {
        sha1_block_data_order(c, ptr, len / SHA_CBLOCK);

        ptr += len;
        c->Nh += len >> 29;
        c->Nl += len <<= 3;
        if (c->Nl < (unsigned int)len)
            c->Nh++;
    }

    if (res)
        SHA1_Update(c, ptr, res);
}