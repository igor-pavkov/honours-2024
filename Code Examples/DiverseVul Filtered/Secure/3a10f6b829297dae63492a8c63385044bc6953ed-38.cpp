void msPostGISCloseConnection(void *pgconn)
{
  PQfinish((PGconn*)pgconn);
}