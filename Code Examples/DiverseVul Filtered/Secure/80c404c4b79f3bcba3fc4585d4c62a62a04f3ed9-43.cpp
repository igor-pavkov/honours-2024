TO_ENTRY_CONN(connection_t *c)
{
  tor_assert(c->magic == ENTRY_CONNECTION_MAGIC);
  return (entry_connection_t*) SUBTYPE_P(c, entry_connection_t, edge_.base_);
}