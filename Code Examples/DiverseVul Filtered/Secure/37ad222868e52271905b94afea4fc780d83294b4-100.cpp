static void addreply_newline(const char * const str, const size_t size)
{
    struct reply *newline;

    if ((newline = (struct reply *) malloc(offsetof(struct reply, line) +
                                           size)) == NULL) {
        die_mem();
    }
    if (firstreply == NULL) {
        firstreply = newline;
    } else {
        lastreply->next = newline;
    }
    newline->next = NULL;
    lastreply = newline;
    memcpy(newline->line, str, size);
}