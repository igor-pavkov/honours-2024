static char* binary_get_key(conn *c) {
    return c->rcurr - (c->binary_header.request.keylen);
}