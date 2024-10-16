static int mb86a20s_i2c_writereg(struct mb86a20s_state *state,
			     u8 i2c_addr, u8 reg, u8 data)
{
	u8 buf[] = { reg, data };
	struct i2c_msg msg = {
		.addr = i2c_addr, .flags = 0, .buf = buf, .len = 2
	};
	int rc;

	rc = i2c_transfer(state->i2c, &msg, 1);
	if (rc != 1) {
		dev_err(&state->i2c->dev,
			"%s: writereg error (rc == %i, reg == 0x%02x, data == 0x%02x)\n",
			__func__, rc, reg, data);
		return rc;
	}

	return 0;
}