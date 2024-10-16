u32 cdk_sk_get_keyid(cdk_pkt_seckey_t sk, u32 * keyid)
{
	u32 lowbits = 0;

	if (sk && sk->pk) {
		lowbits = cdk_pk_get_keyid(sk->pk, keyid);
		sk->keyid[0] = sk->pk->keyid[0];
		sk->keyid[1] = sk->pk->keyid[1];
	}

	return lowbits;
}