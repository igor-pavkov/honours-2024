char *_q_strunchar(char *str, char head, char tail)
{
    if (str == NULL) return NULL;

    int len = strlen(str);
    if (len >= 2 && str[0] == head && str[len-1] == tail) {
        memmove(str, str + 1, len - 2);
        str[len - 2] = '\0';
    }

    return str;
}