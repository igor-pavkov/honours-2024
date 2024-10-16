static int mb86a20s_get_modulation(struct mb86a20s_state *state,
				   unsigned layer)
{
	int rc;
	static unsigned char reg[] = {
		[0] = 0x86,	/* Layer A */
		[1] = 0x8a,	/* Layer B */
		[2] = 0x8e,	/* Layer C */
	};

	if (layer >= ARRAY_SIZE(reg))
		return -EINVAL;
	rc = mb86a20s_writereg(state, 0x6d, reg[layer]);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x6e);
	if (rc < 0)
		return rc;
	switch ((rc >> 4) & 0x07) {
	case 0:
		return DQPSK;
	case 1:
		return QPSK;
	case 2:
		return QAM_16;
	case 3:
		return QAM_64;
	default:
		return QAM_AUTO;
	}
}