NTSTATUS parse_spnego_mechanisms(DATA_BLOB blob_in,
		DATA_BLOB *pblob_out,
		char **kerb_mechOID)
{
	char *OIDs[ASN1_MAX_OIDS];
	int i;
	NTSTATUS ret = NT_STATUS_OK;

	*kerb_mechOID = NULL;

	/* parse out the OIDs and the first sec blob */
	if (!parse_negTokenTarg(blob_in, OIDs, pblob_out)) {
		return NT_STATUS_LOGON_FAILURE;
	}

	/* only look at the first OID for determining the mechToken --
	   according to RFC2478, we should choose the one we want
	   and renegotiate, but i smell a client bug here..

	   Problem observed when connecting to a member (samba box)
	   of an AD domain as a user in a Samba domain.  Samba member
	   server sent back krb5/mskrb5/ntlmssp as mechtypes, but the
	   client (2ksp3) replied with ntlmssp/mskrb5/krb5 and an
	   NTLMSSP mechtoken.                 --jerry              */

#ifdef HAVE_KRB5
	if (strcmp(OID_KERBEROS5, OIDs[0]) == 0 ||
	    strcmp(OID_KERBEROS5_OLD, OIDs[0]) == 0) {
		*kerb_mechOID = SMB_STRDUP(OIDs[0]);
		if (*kerb_mechOID == NULL) {
			ret = NT_STATUS_NO_MEMORY;
		}
	}
#endif

	for (i=0;OIDs[i];i++) {
		DEBUG(5,("parse_spnego_mechanisms: Got OID %s\n", OIDs[i]));
		talloc_free(OIDs[i]);
	}
	return ret;
}