void udscs_server_destroy_connection(struct udscs_server *server,
                                     UdscsConnection     *conn)
{
    server->connections = g_list_remove(server->connections, conn);
    vdagent_connection_destroy(conn);
}