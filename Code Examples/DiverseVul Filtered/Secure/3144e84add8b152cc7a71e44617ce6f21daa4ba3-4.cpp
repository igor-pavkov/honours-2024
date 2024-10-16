static void push_mmacro(MMacro *m)
{
    MMacroInvocation *i;

    i = nasm_malloc(sizeof(MMacroInvocation));
    i->prev = m->prev;
    i->params = m->params;
    i->iline = m->iline;
    i->nparam = m->nparam;
    i->rotate = m->rotate;
    i->paramlen = m->paramlen;
    i->unique = m->unique;
    i->condcnt = m->condcnt;
    m->prev = i;
}