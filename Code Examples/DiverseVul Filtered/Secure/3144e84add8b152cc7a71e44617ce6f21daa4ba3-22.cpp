static Token *new_Token(Token * next, enum pp_token_type type,
                        const char *text, int txtlen)
{
    Token *t;
    int i;

    if (!freeTokens) {
        freeTokens = (Token *) new_Block(TOKEN_BLOCKSIZE * sizeof(Token));
        for (i = 0; i < TOKEN_BLOCKSIZE - 1; i++)
            freeTokens[i].next = &freeTokens[i + 1];
        freeTokens[i].next = NULL;
    }
    t = freeTokens;
    freeTokens = t->next;
    t->next = next;
    t->a.mac = NULL;
    t->type = type;
    if (type == TOK_WHITESPACE || !text) {
        t->text = NULL;
    } else {
        if (txtlen == 0)
            txtlen = strlen(text);
        t->text = nasm_malloc(txtlen+1);
        memcpy(t->text, text, txtlen);
        t->text[txtlen] = '\0';
    }
    return t;
}