void addreply_noformat(const int code, const char * const line)
{
    if (code != 0) {
        replycode = code;
    }
    addreply_newline(line, strlen(line) + (size_t) 1U);
}