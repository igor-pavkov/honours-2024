static void debug_print_message_header(UdscsConnection             *conn,
                                       struct udscs_message_header *header,
                                       const gchar                 *direction)
{
    const gchar *type = "invalid message";

    if (conn == NULL || conn->debug == FALSE)
        return;

    if (header->type < G_N_ELEMENTS(vdagentd_messages))
        type = vdagentd_messages[header->type];

    syslog(LOG_DEBUG, "%p %s %s, arg1: %u, arg2: %u, size %u",
        conn, direction, type, header->arg1, header->arg2, header->size);
}