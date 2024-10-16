void addReplyError(client *c, const char *err) {
    addReplyErrorLength(c,err,strlen(err));
}