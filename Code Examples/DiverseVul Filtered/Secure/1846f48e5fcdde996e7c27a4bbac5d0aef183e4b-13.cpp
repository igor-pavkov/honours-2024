void error_handler(int priority, const char *format, ...)
{
    gdTestAssert(priority == GD_WARNING);
    gdTestAssert(!strcmp(format, MSG));
}