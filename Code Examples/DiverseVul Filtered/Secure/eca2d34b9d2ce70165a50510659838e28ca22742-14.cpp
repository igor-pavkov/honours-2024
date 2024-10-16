static int mb86a20s_get_fec(struct mb86a20s_state *state,
			    unsigned layer)
{
	int rc;

	static unsigned char reg[] = {
		[0] = 0x87,	/* Layer A */
		[1] = 0x8b,	/* Layer B */
		[2] = 0x8f,	/* Layer C */
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
		return FEC_1_2;
	case 1:
		return FEC_2_3;
	case 2:
		return FEC_3_4;
	case 3:
		return FEC_5_6;
	case 4:
		return FEC_7_8;
	default:
		return FEC_AUTO;
	}
}