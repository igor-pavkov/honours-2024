char *_q_makeword(char *str, char stop)
{
    char *word;
    int  len, i;

    for (len = 0; ((str[len] != stop) && (str[len])); len++);
    word = (char *)malloc(sizeof(char) * (len + 1));

    for (i = 0; i < len; i++) word[i] = str[i];
    word[i] = '\0';

    if (str[len])len++;
    for (i = len; str[i]; i++) str[i - len] = str[i];
    str[i - len] = '\0';

    return word;
}