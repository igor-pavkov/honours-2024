void udscs_server_write_all(struct udscs_server *server,
        uint32_t type, uint32_t arg1, uint32_t arg2,
        const uint8_t *data, uint32_t size)
{
    GList *l;
    for (l = server->connections; l; l = l->next) {
        udscs_write(UDSCS_CONNECTION(l->data), type, arg1, arg2, data, size);
    }
}