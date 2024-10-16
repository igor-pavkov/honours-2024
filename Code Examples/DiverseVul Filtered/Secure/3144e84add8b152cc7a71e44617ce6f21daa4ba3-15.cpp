static Token *delete_Token(Token * t)
{
    Token *next = t->next;
    nasm_free(t->text);
    t->next = freeTokens;
    freeTokens = t;
    return next;
}