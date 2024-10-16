static void _write_acl(struct dlist *dl, const char *aclstr)
{
    const char *p, *q;
    struct dlist *al = dlist_newkvlist(dl, "A");

    p = aclstr;

    while (p && *p) {
        char *name,*val;

        q = strchr(p, '\t');
        if (!q) break;

        name = xstrndup(p, q-p);
        q++;

        p = strchr(q, '\t');
        if (p) {
            val = xstrndup(q, p-q);
            p++;
        }
        else
            val = xstrdup(q);

        dlist_setatom(al, name, val);

        free(name);
        free(val);
    }
}