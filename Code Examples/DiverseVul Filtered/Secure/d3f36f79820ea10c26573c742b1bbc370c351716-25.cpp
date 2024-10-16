SSLNetVConnection::populate(Connection &con, Continuation *c, void *arg)
{
  int retval = super::populate(con, c, arg);
  if (retval != EVENT_DONE) {
    return retval;
  }
  // Add in the SSL data
  this->ssl = (SSL *)arg;
  // Maybe bring over the stats?

  this->sslHandShakeComplete = true;
  SSLNetVCAttach(this->ssl, this);
  return EVENT_DONE;
}