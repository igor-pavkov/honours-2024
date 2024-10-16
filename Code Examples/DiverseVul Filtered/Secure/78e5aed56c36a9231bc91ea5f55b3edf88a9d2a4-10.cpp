static int set_ecdh_curve(SSL_CTX *ctx, const char *ecdh_curve)
{
	int      nid; 
	EC_KEY  *ecdh; 

	if (!ecdh_curve || !*ecdh_curve) return 0;

	nid = OBJ_sn2nid(ecdh_curve); 
	if (!nid) { 
		radlog(L_ERR, "Unknown ecdh_curve \"%s\"", ecdh_curve);
		return -1;
	}

	ecdh = EC_KEY_new_by_curve_name(nid); 
	if (!ecdh) { 
		radlog(L_ERR, "Unable to create new curve \"%s\"", ecdh_curve);
		return -1;
	} 

	SSL_CTX_set_tmp_ecdh(ctx, ecdh); 

	SSL_CTX_set_options(ctx, SSL_OP_SINGLE_ECDH_USE); 

	EC_KEY_free(ecdh);

	return 0;
}