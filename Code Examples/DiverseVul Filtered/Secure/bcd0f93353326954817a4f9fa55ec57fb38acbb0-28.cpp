static int pipe_snd_status(struct sock *sk, u8 type, u8 status, gfp_t priority)
{
	u8 data[4] = { type, PAD, PAD, status };

	return pep_indicate(sk, PNS_PEP_STATUS_IND, PN_PEP_TYPE_COMMON,
				data, 4, priority);
}