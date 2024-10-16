static int mb86a20s_i2c_readreg(struct mb86a20s_state *state,
				u8 i2c_addr, u8 reg)
{
	u8 val;
	int rc;
	struct i2c_msg msg[] = {
		{ .addr = i2c_addr, .flags = 0, .buf = &reg, .len = 1 },
		{ .addr = i2c_addr, .flags = I2C_M_RD, .buf = &val, .len = 1 }
	};

	rc = i2c_transfer(state->i2c, msg, 2);

	if (rc != 2) {
		dev_err(&state->i2c->dev, "%s: reg=0x%x (error=%d)\n",
			__func__, reg, rc);
		return (rc < 0) ? rc : -EIO;
	}

	return val;
}