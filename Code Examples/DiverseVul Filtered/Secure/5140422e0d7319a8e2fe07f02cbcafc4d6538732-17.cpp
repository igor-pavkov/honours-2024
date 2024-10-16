u32 _cdk_pkt_get_keyid(cdk_packet_t pkt, u32 * keyid)
{
	u32 lowbits;

	if (!pkt)
		return 0;

	switch (pkt->pkttype) {
	case CDK_PKT_PUBLIC_KEY:
	case CDK_PKT_PUBLIC_SUBKEY:
		lowbits = cdk_pk_get_keyid(pkt->pkt.public_key, keyid);
		break;

	case CDK_PKT_SECRET_KEY:
	case CDK_PKT_SECRET_SUBKEY:
		lowbits = cdk_sk_get_keyid(pkt->pkt.secret_key, keyid);
		break;

	case CDK_PKT_SIGNATURE:
		lowbits = cdk_sig_get_keyid(pkt->pkt.signature, keyid);
		break;

	default:
		lowbits = 0;
		break;
	}

	return lowbits;
}