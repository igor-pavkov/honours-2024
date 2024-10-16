cdk_error_t _cdk_pkt_get_fingerprint(cdk_packet_t pkt, byte * fpr)
{
	if (!pkt || !fpr)
		return CDK_Inv_Value;

	switch (pkt->pkttype) {
	case CDK_PKT_PUBLIC_KEY:
	case CDK_PKT_PUBLIC_SUBKEY:
		return cdk_pk_get_fingerprint(pkt->pkt.public_key, fpr);

	case CDK_PKT_SECRET_KEY:
	case CDK_PKT_SECRET_SUBKEY:
		return cdk_pk_get_fingerprint(pkt->pkt.secret_key->pk,
					      fpr);

	default:
		return CDK_Inv_Mode;
	}
	return 0;
}