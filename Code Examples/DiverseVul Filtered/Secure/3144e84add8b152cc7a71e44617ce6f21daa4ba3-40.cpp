static void pp_pre_define(char *definition)
{
    Token *def, *space;
    Line *l;
    char *equals;

    real_verror = nasm_set_verror(pp_verror);

    equals = strchr(definition, '=');
    space = new_Token(NULL, TOK_WHITESPACE, NULL, 0);
    def = new_Token(space, TOK_PREPROC_ID, "%define", 0);
    if (equals)
        *equals = ' ';
    space->next = tokenize(definition);
    if (equals)
        *equals = '=';

    if (space->next->type != TOK_PREPROC_ID &&
        space->next->type != TOK_ID)
        nasm_error(ERR_WARNING, "pre-defining non ID `%s\'\n", definition);

    l = nasm_malloc(sizeof(Line));
    l->next = predef;
    l->first = def;
    l->finishes = NULL;
    predef = l;

    nasm_set_verror(real_verror);
}