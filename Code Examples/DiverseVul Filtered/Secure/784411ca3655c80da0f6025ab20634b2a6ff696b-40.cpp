static field_t *parse_error(field_t * rv, char *port)
{
    free_field(rv);
    if (port)
	free(port);
    return NULL;
}