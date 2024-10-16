void addReplyStatus(client *c, const char *status) {
    addReplyStatusLength(c,status,strlen(status));
}