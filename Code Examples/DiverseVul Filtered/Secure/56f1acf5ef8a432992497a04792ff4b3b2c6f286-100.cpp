static void ssl_buf_freelist_free(SSL3_BUF_FREELIST *list)
{
    SSL3_BUF_FREELIST_ENTRY *ent, *next;
    for (ent = list->head; ent; ent = next) {
        next = ent->next;
        OPENSSL_free(ent);
    }
    OPENSSL_free(list);
}