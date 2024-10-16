static void make_tok_num(Token * tok, int64_t val)
{
    char numbuf[32];
    snprintf(numbuf, sizeof(numbuf), "%"PRId64"", val);
    tok->text = nasm_strdup(numbuf);
    tok->type = TOK_NUMBER;
}