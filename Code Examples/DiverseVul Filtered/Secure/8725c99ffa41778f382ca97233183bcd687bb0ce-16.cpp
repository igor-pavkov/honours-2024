public(gcry_mpi_t output, gcry_mpi_t input, RSA_public_key *pkey )
{
  if( output == input )  /* powm doesn't like output and input the same */
    {
      gcry_mpi_t x = mpi_alloc( mpi_get_nlimbs(input)*2 );
      mpi_powm( x, input, pkey->e, pkey->n );
      mpi_set(output, x);
      mpi_free(x);
    }
  else
    mpi_powm( output, input, pkey->e, pkey->n );
}