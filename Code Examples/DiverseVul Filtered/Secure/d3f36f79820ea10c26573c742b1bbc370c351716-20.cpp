SSLNetVConnection::update_rbio(bool move_to_socket)
{
  bool retval = false;
  if (BIO_eof(SSL_get_rbio(this->ssl))) {
    this->handShakeReader->consume(this->handShakeBioStored);
    this->handShakeBioStored = 0;
    // Load up the next block if present
    if (this->handShakeReader->is_read_avail_more_than(0)) {
      // Setup the next iobuffer block to drain
      char *start              = this->handShakeReader->start();
      char *end                = this->handShakeReader->end();
      this->handShakeBioStored = end - start;

      // Sets up the buffer as a read only bio target
      // Must be reset on each read
      BIO *rbio = BIO_new_mem_buf(start, this->handShakeBioStored);
      BIO_set_mem_eof_return(rbio, -1);
      SSL_set0_rbio(this->ssl, rbio);
      retval = true;
      // Handshake buffer is empty but we have read something, move to the socket rbio
    } else if (move_to_socket && this->handShakeHolder->is_read_avail_more_than(0)) {
      BIO *rbio = BIO_new_fd(this->get_socket(), BIO_NOCLOSE);
      BIO_set_mem_eof_return(rbio, -1);
      SSL_set0_rbio(this->ssl, rbio);
      free_handshake_buffers();
    }
  }
  return retval;
}