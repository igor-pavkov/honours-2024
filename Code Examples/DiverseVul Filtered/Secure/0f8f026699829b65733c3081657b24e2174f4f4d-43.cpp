static unsigned expunge_cancelled(struct mailbox *mailbox __attribute__((unused)),
				  struct index_record *record,
				  void *rock)
{
    /* only expunge the UID that we obtained from the msgid */
    return (record->uid == *((unsigned long *) rock));
}