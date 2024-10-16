u32 cdk_sig_get_keyid(cdk_pkt_signature_t sig, u32 * keyid)
{
	u32 lowbits = sig ? sig->keyid[1] : 0;

	if (keyid && sig) {
		keyid[0] = sig->keyid[0];
		keyid[1] = sig->keyid[1];
	}
	return lowbits;
}