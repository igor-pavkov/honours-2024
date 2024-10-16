static void poly_free(node_t * n)
{
    polygon_t *p = ND_shape_info(n);

    if (p) {
	free(p->vertices);
	free(p);
    }
}