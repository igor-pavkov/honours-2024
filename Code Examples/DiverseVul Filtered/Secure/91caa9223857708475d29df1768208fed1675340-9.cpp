int udscs_server_for_all_clients(struct udscs_server *server,
    udscs_for_all_clients_callback func, void *priv)
{
    int r = 0;
    GList *l, *next;

    if (!server)
        return 0;

    l = server->connections;
    while (l) {
        next = l->next;
        r += func(l->data, priv);
        l = next;
    }
    return r;
}