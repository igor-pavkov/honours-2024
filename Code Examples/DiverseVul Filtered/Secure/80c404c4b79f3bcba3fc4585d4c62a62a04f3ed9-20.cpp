EDGE_TO_ENTRY_CONN(edge_connection_t *c)
{
  tor_assert(c->base_.magic == ENTRY_CONNECTION_MAGIC);
  return (entry_connection_t*) SUBTYPE_P(c, entry_connection_t, edge_);
}