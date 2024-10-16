static gsize conn_handle_header(VDAgentConnection *conn,
                                gpointer           header_buf)
{
    return ((struct udscs_message_header *)header_buf)->size;
}