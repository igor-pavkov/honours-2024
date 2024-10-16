static char num2hex(char c)
{
    if (c < 10)
        return '0' + c;
    return 'a' + c - 10;
}