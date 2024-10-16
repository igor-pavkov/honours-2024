TO_EDGE_CONN(connection_t *c)
{
  tor_assert(c->magic == EDGE_CONNECTION_MAGIC ||
             c->magic == ENTRY_CONNECTION_MAGIC);
  return DOWNCAST(edge_connection_t, c);
}