void addReplyBulkSds(client *c, sds s)  {
    addReplySds(c,sdscatfmt(sdsempty(),"$%u\r\n",
        (unsigned long)sdslen(s)));
    addReplySds(c,s);
    addReply(c,shared.crlf);
}