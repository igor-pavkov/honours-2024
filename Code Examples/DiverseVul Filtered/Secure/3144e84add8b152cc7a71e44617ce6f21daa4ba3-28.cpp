static void pop_mmacro(MMacro *m)
{
    MMacroInvocation *i;

    if (m->prev) {
        i = m->prev;
        m->prev = i->prev;
        m->params = i->params;
        m->iline = i->iline;
        m->nparam = i->nparam;
        m->rotate = i->rotate;
        m->paramlen = i->paramlen;
        m->unique = i->unique;
        m->condcnt = i->condcnt;
        nasm_free(i);
    }
}