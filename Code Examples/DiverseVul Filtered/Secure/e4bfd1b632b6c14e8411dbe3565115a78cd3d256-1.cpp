static int connection_matches_session(UdscsConnection *conn, void *priv)
{
    const char *session = priv;
    const struct agent_data *agent_data = g_object_get_data(G_OBJECT(conn), "agent_data");

    if (!agent_data || !agent_data->session ||
        strcmp(agent_data->session, session) != 0) {
        return 0;
    }

    return 1;
}