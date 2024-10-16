static bool atl2_read_eeprom(struct atl2_hw *hw, u32 Offset, u32 *pValue)
{
	int i;
	u32    Control;

	if (Offset & 0x3)
		return false; /* address do not align */

	ATL2_WRITE_REG(hw, REG_VPD_DATA, 0);
	Control = (Offset & VPD_CAP_VPD_ADDR_MASK) << VPD_CAP_VPD_ADDR_SHIFT;
	ATL2_WRITE_REG(hw, REG_VPD_CAP, Control);

	for (i = 0; i < 10; i++) {
		msleep(2);
		Control = ATL2_READ_REG(hw, REG_VPD_CAP);
		if (Control & VPD_CAP_VPD_FLAG)
			break;
	}

	if (Control & VPD_CAP_VPD_FLAG) {
		*pValue = ATL2_READ_REG(hw, REG_VPD_DATA);
		return true;
	}
	return false; /* timeout */
}