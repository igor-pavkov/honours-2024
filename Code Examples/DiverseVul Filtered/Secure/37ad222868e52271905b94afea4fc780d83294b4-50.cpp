static int ul_exit(ULHandler * const ulhandler)
{
    free(ulhandler->buf);
    ulhandler->buf = NULL;

    return 0;
}