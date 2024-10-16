static int ocsp_parse_cert_url(X509 *cert, char **phost, char **pport,
			       char **ppath, int *pssl)
{
	int i;
	
	AUTHORITY_INFO_ACCESS *aia;
	ACCESS_DESCRIPTION *ad;
	
	aia = X509_get_ext_d2i(cert, NID_info_access, NULL, NULL);

	for (i = 0; i < sk_ACCESS_DESCRIPTION_num(aia); i++) {
		ad = sk_ACCESS_DESCRIPTION_value(aia, 0);
		if (OBJ_obj2nid(ad->method) == NID_ad_OCSP) {
			if (ad->location->type == GEN_URI) {
				if(OCSP_parse_url(ad->location->d.ia5->data, 
					phost, pport, ppath, pssl))
					return 1;
			}
		}
	}
	return 0;
}