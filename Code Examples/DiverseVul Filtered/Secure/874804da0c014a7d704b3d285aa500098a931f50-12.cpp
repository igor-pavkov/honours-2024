sds getAllClientsInfoString(void) {
    listNode *ln;
    listIter li;
    client *client;
    sds o = sdsnewlen(NULL,200*listLength(server.clients));
    sdsclear(o);
    listRewind(server.clients,&li);
    while ((ln = listNext(&li)) != NULL) {
        client = listNodeValue(ln);
        o = catClientInfoString(o,client);
        o = sdscatlen(o,"\n",1);
    }
    return o;
}