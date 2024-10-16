void limit_string(char *name)
{
    int     i;

    if (!name)
        return;

    if (strlen(name) < NAME_LENGTH_LIMIT)
        return;

    i = NAME_LENGTH_LIMIT-4;
    name[i++] = '.';
    name[i++] = '.';
    name[i++] = '.';
    name[i] = '\0';
    return;
}