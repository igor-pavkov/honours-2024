static void pp_pre_undefine(char *definition)
{
    Token *def, *space;
    Line *l;

    space = new_Token(NULL, TOK_WHITESPACE, NULL, 0);
    def = new_Token(space, TOK_PREPROC_ID, "%undef", 0);
    space->next = tokenize(definition);

    l = nasm_malloc(sizeof(Line));
    l->next = predef;
    l->first = def;
    l->finishes = NULL;
    predef = l;
}