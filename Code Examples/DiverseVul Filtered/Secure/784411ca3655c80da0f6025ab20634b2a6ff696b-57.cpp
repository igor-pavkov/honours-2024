boolean isPolygon(node_t * n)
{
    return (ND_shape(n) && (ND_shape(n)->fns->initfn == poly_init));
}