static void connection_fdwaitqueue_append(connection *con) {
    connection_list_append(&con->srv->fdwaitqueue, con);
}