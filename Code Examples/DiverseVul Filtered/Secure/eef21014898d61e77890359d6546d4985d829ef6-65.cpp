static int ssl_verify_server_cert(Vio *vio, const char* server_hostname, const char **errptr)
{
  SSL *ssl;
  X509 *server_cert= NULL;
  char *cn= NULL;
  int cn_loc= -1;
  ASN1_STRING *cn_asn1= NULL;
  X509_NAME_ENTRY *cn_entry= NULL;
  X509_NAME *subject= NULL;
  int ret_validation= 1;

  DBUG_ENTER("ssl_verify_server_cert");
  DBUG_PRINT("enter", ("server_hostname: %s", server_hostname));

  if (!(ssl= (SSL*)vio->ssl_arg))
  {
    *errptr= "No SSL pointer found";
    goto error;
  }

  if (!server_hostname)
  {
    *errptr= "No server hostname supplied";
    goto error;
  }

  if (!(server_cert= SSL_get_peer_certificate(ssl)))
  {
    *errptr= "Could not get server certificate";
    goto error;
  }

  if (X509_V_OK != SSL_get_verify_result(ssl))
  {
    *errptr= "Failed to verify the server certificate";
    goto error;
  }
  /*
    We already know that the certificate exchanged was valid; the SSL library
    handled that. Now we need to verify that the contents of the certificate
    are what we expect.
  */

  /*
   Some notes for future development
   We should check host name in alternative name first and then if needed check in common name.
   Currently yssl doesn't support alternative name.
   openssl 1.0.2 support X509_check_host method for host name validation, we may need to start using
   X509_check_host in the future.
  */

  subject= X509_get_subject_name(server_cert);
  cn_loc= X509_NAME_get_index_by_NID(subject, NID_commonName, -1);
  if (cn_loc < 0)
  {
    *errptr= "Failed to get CN location in the certificate subject";
    goto error;
  }

  cn_entry= X509_NAME_get_entry(subject, cn_loc);
  if (cn_entry == NULL)
  {
    *errptr= "Failed to get CN entry using CN location";
    goto error;
  }

  cn_asn1 = X509_NAME_ENTRY_get_data(cn_entry);
  if (cn_asn1 == NULL)
  {
    *errptr= "Failed to get CN from CN entry";
    goto error;
  }

  cn= (char *) ASN1_STRING_data(cn_asn1);

  if ((size_t)ASN1_STRING_length(cn_asn1) != strlen(cn))
  {
    *errptr= "NULL embedded in the certificate CN";
    goto error;
  }

  DBUG_PRINT("info", ("Server hostname in cert: %s", cn));
  if (!strcmp(cn, server_hostname))
  {
    /* Success */
    ret_validation= 0;
  }

  *errptr= "SSL certificate validation failure";

error:
  if (server_cert != NULL)
    X509_free (server_cert);
  DBUG_RETURN(ret_validation);
}