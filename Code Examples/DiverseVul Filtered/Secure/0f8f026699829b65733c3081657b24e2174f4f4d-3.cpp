static int cancel(message_data_t *msg)
{
    int r = 0;
    char *msgid, *p;

    /* isolate msgid */
    msgid = strchr(msg->control, '<');
    p = strrchr(msgid, '>') + 1;
    *p = '\0';

    /* find and expunge the message from all mailboxes */
    duplicate_find(msgid, &cancel_cb, &r);

    /* store msgid of cancelled message for IHAVE/CHECK/TAKETHIS
     * (in case we haven't received the message yet)
     */
    duplicate_key_t dkey = {msgid, "", ""};
    duplicate_mark(&dkey, 0, time(NULL));

    return r;
}