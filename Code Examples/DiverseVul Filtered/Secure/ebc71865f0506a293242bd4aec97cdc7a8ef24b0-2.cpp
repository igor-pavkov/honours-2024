static int ocsp_match_issuerid(X509 *cert, OCSP_CERTID *cid,
			STACK_OF(OCSP_SINGLERESP) *sresp)
	{
	/* If only one ID to match then do it */
	if(cid)
		{
		const EVP_MD *dgst;
		X509_NAME *iname;
		int mdlen;
		unsigned char md[EVP_MAX_MD_SIZE];
		if (!(dgst = EVP_get_digestbyobj(cid->hashAlgorithm->algorithm)))
			{
			OCSPerr(OCSP_F_OCSP_MATCH_ISSUERID, OCSP_R_UNKNOWN_MESSAGE_DIGEST);
			return -1;
			}

		mdlen = EVP_MD_size(dgst);
		if (mdlen < 0)
		    return -1;
		if ((cid->issuerNameHash->length != mdlen) ||
		   (cid->issuerKeyHash->length != mdlen))
			return 0;
		iname = X509_get_subject_name(cert);
		if (!X509_NAME_digest(iname, dgst, md, NULL))
			return -1;
		if (memcmp(md, cid->issuerNameHash->data, mdlen))
			return 0;
		X509_pubkey_digest(cert, dgst, md, NULL);
		if (memcmp(md, cid->issuerKeyHash->data, mdlen))
			return 0;

		return 1;

		}
	else
		{
		/* We have to match the whole lot */
		int i, ret;
		OCSP_CERTID *tmpid;
		for (i = 0; i < sk_OCSP_SINGLERESP_num(sresp); i++)
			{
			tmpid = sk_OCSP_SINGLERESP_value(sresp, i)->certId;
			ret = ocsp_match_issuerid(cert, tmpid, NULL);
			if (ret <= 0) return ret;
			}
		return 1;
		}
			
	}