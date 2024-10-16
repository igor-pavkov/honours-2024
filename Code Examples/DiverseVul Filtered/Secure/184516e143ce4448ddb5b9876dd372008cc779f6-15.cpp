static u_int32 derive_nonce(
	sockaddr_u *	addr,
	u_int32		ts_i,
	u_int32		ts_f
	)
{
	static u_int32	salt[4];
	static u_long	last_salt_update;
	union d_tag {
		u_char	digest[EVP_MAX_MD_SIZE];
		u_int32 extract;
	}		d;
	EVP_MD_CTX	ctx;
	u_int		len;

	while (!salt[0] || current_time - last_salt_update >= 3600) {
		salt[0] = ntp_random();
		salt[1] = ntp_random();
		salt[2] = ntp_random();
		salt[3] = ntp_random();
		last_salt_update = current_time;
	}

	EVP_DigestInit(&ctx, EVP_get_digestbynid(NID_md5));
	EVP_DigestUpdate(&ctx, salt, sizeof(salt));
	EVP_DigestUpdate(&ctx, &ts_i, sizeof(ts_i));
	EVP_DigestUpdate(&ctx, &ts_f, sizeof(ts_f));
	if (IS_IPV4(addr))
		EVP_DigestUpdate(&ctx, &SOCK_ADDR4(addr),
			         sizeof(SOCK_ADDR4(addr)));
	else
		EVP_DigestUpdate(&ctx, &SOCK_ADDR6(addr),
			         sizeof(SOCK_ADDR6(addr)));
	EVP_DigestUpdate(&ctx, &NSRCPORT(addr), sizeof(NSRCPORT(addr)));
	EVP_DigestUpdate(&ctx, salt, sizeof(salt));
	EVP_DigestFinal(&ctx, d.digest, &len);

	return d.extract;
}