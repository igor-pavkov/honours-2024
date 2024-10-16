static void process_slabs_automove_command(conn *c, token_t *tokens, const size_t ntokens) {
    unsigned int level;

    assert(c != NULL);

    set_noreply_maybe(c, tokens, ntokens);

    level = strtoul(tokens[2].value, NULL, 10);
    if (level == 0) {
        settings.slab_automove = 0;
    } else if (level == 1 || level == 2) {
        settings.slab_automove = level;
    } else {
        out_string(c, "ERROR");
        return;
    }
    out_string(c, "OK");
    return;
}