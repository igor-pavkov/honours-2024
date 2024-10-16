static void setProtocolError(client *c, int pos) {
    if (server.verbosity <= LL_VERBOSE) {
        sds client = catClientInfoString(sdsempty(),c);
        serverLog(LL_VERBOSE,
            "Protocol error from client: %s", client);
        sdsfree(client);
    }
    c->flags |= CLIENT_CLOSE_AFTER_REPLY;
    sdsrange(c->querybuf,pos,-1);
}