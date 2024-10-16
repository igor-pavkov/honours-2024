static int create_MD5_sum(request_st * const r) {
    uint32_t buf[4]; /* MD5 binary hash len */

    const buffer *key1 =
      http_header_request_get(r, HTTP_HEADER_OTHER, CONST_STR_LEN("Sec-WebSocket-Key1"));
    const buffer *key2 =
      http_header_request_get(r, HTTP_HEADER_OTHER, CONST_STR_LEN("Sec-WebSocket-Key2"));

    if (NULL == key1 || get_key_number(buf+0, key1) < 0 ||
        NULL == key2 || get_key_number(buf+1, key2) < 0 ||
        get_key3(r, (char *)(buf+2), 2*sizeof(uint32_t)) < 0) {
        return -1;
    }
  #ifdef __BIG_ENDIAN__
  #define ws_htole32(s,u)\
    (s)[0]=((u)>>24);    \
    (s)[1]=((u)>>16);    \
    (s)[2]=((u)>>8);     \
    (s)[3]=((u))
    ws_htole32((unsigned char *)(buf+0), buf[0]);
    ws_htole32((unsigned char *)(buf+1), buf[1]);
  #endif
    /*(overwrite buf[] with result)*/
    MD5_once((unsigned char *)buf, buf, sizeof(buf));
    chunkqueue_append_mem(&r->write_queue, (char *)buf, sizeof(buf));
    return 0;
}