char *_q_strtrim(char *str)
{
    int i, j;

    if (str == NULL) return NULL;
    for (j = 0; str[j] == ' ' || str[j] == '\t' || str[j] == '\r' || str[j] == '\n'; j++);
    for (i = 0; str[j] != '\0'; i++, j++) str[i] = str[j];
    for (i--; (i >= 0) && (str[i] == ' ' || str[i] == '\t' || str[i] == '\r' || str[i] == '\n'); i--);
    str[i+1] = '\0';

    return str;
}