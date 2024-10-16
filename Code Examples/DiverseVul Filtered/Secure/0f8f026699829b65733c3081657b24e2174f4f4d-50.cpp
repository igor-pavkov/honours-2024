static char *nntp_parsesuccess(char *str, const char **status)
{
    char *success = NULL;

    if (!strncmp(str, "283 ", 4)) {
	success = str+4;
    }

    if (status) *status = NULL;
    return success;
}