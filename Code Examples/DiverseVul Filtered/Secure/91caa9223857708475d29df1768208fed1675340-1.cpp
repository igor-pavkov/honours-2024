static void udscs_connection_class_init(UdscsConnectionClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    VDAgentConnectionClass *conn_class = VDAGENT_CONNECTION_CLASS(klass);

    gobject_class->finalize = udscs_connection_finalize;

    conn_class->handle_header = conn_handle_header;
    conn_class->handle_message = conn_handle_message;
}