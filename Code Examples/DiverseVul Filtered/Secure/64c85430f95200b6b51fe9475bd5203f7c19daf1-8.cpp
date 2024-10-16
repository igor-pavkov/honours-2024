static int expand(OPENSSL_LHASH *lh)
{
    OPENSSL_LH_NODE **n, **n1, **n2, *np;
    unsigned int p, pmax, nni, j;
    unsigned long hash;

    nni = lh->num_alloc_nodes;
    p = lh->p;
    pmax = lh->pmax;
    if (p + 1 >= pmax) {
        j = nni * 2;
        n = OPENSSL_realloc(lh->b, sizeof(OPENSSL_LH_NODE *) * j);
        if (n == NULL) {
            lh->error++;
            return 0;
        }
        lh->b = n;
        memset(n + nni, 0, sizeof(*n) * (j - nni));
        lh->pmax = nni;
        lh->num_alloc_nodes = j;
        lh->p = 0;
    } else {
        lh->p++;
    }

    lh->num_nodes++;
    n1 = &(lh->b[p]);
    n2 = &(lh->b[p + pmax]);
    *n2 = NULL;

    for (np = *n1; np != NULL;) {
        hash = np->hash;
        if ((hash % nni) != p) { /* move it */
            *n1 = (*n1)->next;
            np->next = *n2;
            *n2 = np;
        } else
            n1 = &((*n1)->next);
        np = *n1;
    }

    return 1;
}