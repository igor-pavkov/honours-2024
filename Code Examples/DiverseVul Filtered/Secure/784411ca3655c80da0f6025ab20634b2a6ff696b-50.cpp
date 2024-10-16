static void record_free(node_t * n)
{
    field_t *p = ND_shape_info(n);

    free_field(p);
}