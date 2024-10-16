static int mb86a20s_get_interleaving(struct mb86a20s_state *state,
				     unsigned layer)
{
	int rc;
	int interleaving[] = {
		0, 1, 2, 4, 8
	};

	static unsigned char reg[] = {
		[0] = 0x88,	/* Layer A */
		[1] = 0x8c,	/* Layer B */
		[2] = 0x90,	/* Layer C */
	};

	if (layer >= ARRAY_SIZE(reg))
		return -EINVAL;
	rc = mb86a20s_writereg(state, 0x6d, reg[layer]);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x6e);
	if (rc < 0)
		return rc;

	return interleaving[(rc >> 4) & 0x07];
}