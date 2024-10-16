static int atusb_read_subreg(struct atusb *lp,
			     unsigned int addr, unsigned int mask,
			     unsigned int shift)
{
	int rc;

	rc = atusb_read_reg(lp, addr);
	rc = (rc & mask) >> shift;

	return rc;
}