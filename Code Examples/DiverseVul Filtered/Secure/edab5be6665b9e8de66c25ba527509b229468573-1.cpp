static bool send_client_authreq(PgSocket *client)
{
	uint8_t saltlen = 0;
	int res;
	int auth = cf_auth_type;
	uint8_t randbuf[2];

	if (auth == AUTH_CRYPT) {
		saltlen = 2;
		get_random_bytes(randbuf, saltlen);
		client->tmp_login_salt[0] = valid_crypt_salt[randbuf[0] & SALT_MASK];
		client->tmp_login_salt[1] = valid_crypt_salt[randbuf[1] & SALT_MASK];
		client->tmp_login_salt[2] = 0;
	} else if (cf_auth_type == AUTH_MD5) {
		saltlen = 4;
		get_random_bytes((void*)client->tmp_login_salt, saltlen);
	} else if (auth == AUTH_ANY)
		auth = AUTH_TRUST;

	SEND_generic(res, client, 'R', "ib", auth, client->tmp_login_salt, saltlen);
	return res;
}