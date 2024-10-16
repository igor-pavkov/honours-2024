static void conn_handle_message(VDAgentConnection *conn,
                                gpointer           header_buf,
                                gpointer           data)
{
    UdscsConnection *self = UDSCS_CONNECTION(conn);
    struct udscs_message_header *header = header_buf;

    debug_print_message_header(self, header, "received");

    self->read_callback(self, header, data);
}