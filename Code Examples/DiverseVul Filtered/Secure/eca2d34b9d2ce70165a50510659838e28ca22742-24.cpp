static int mb86a20s_get_segment_count(struct mb86a20s_state *state,
				      unsigned layer)
{
	int rc, count;
	static unsigned char reg[] = {
		[0] = 0x89,	/* Layer A */
		[1] = 0x8d,	/* Layer B */
		[2] = 0x91,	/* Layer C */
	};

	dev_dbg(&state->i2c->dev, "%s called.\n", __func__);

	if (layer >= ARRAY_SIZE(reg))
		return -EINVAL;

	rc = mb86a20s_writereg(state, 0x6d, reg[layer]);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x6e);
	if (rc < 0)
		return rc;
	count = (rc >> 4) & 0x0f;

	dev_dbg(&state->i2c->dev, "%s: segments: %d.\n", __func__, count);

	return count;
}