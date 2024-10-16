static boolean epsf_inside(inside_t * inside_context, pointf p)
{
    pointf P;
    double x2;
    node_t *n = inside_context->s.n;

    P = ccwrotatepf(p, 90 * GD_rankdir(agraphof(n)));
    x2 = ND_ht(n) / 2;
    return ((P.y >= -x2) && (P.y <= x2) && (P.x >= -ND_lw(n))
	    && (P.x <= ND_rw(n)));
}