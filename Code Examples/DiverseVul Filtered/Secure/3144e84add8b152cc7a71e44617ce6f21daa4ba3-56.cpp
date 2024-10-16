static Token *reverse_tokens(Token *t)
{
    Token *prev = NULL;
    Token *next;

    while (t) {
        next = t->next;
        t->next = prev;
        prev = t;
        t = next;
    }

    return prev;
}