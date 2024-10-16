static int find_cc(Token * t)
{
    Token *tt;

    if (!t)
        return -1;              /* Probably a %+ without a space */

    skip_white_(t);
    if (t->type != TOK_ID)
        return -1;
    tt = t->next;
    skip_white_(tt);
    if (tt && (tt->type != TOK_OTHER || strcmp(tt->text, ",")))
        return -1;

    return bsii(t->text, (const char **)conditions,  ARRAY_SIZE(conditions));
}