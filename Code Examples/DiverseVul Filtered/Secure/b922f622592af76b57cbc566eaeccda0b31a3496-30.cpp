void hw_atl_utils_hw_chip_features_init(struct aq_hw_s *self, u32 *p)
{
	u32 val = hw_atl_reg_glb_mif_id_get(self);
	u32 mif_rev = val & 0xFFU;
	u32 chip_features = 0U;

	chip_features |= ATL_HW_CHIP_ATLANTIC;

	if ((0xFU & mif_rev) == 1U) {
		chip_features |= ATL_HW_CHIP_REVISION_A0 |
			ATL_HW_CHIP_MPI_AQ |
			ATL_HW_CHIP_MIPS;
	} else if ((0xFU & mif_rev) == 2U) {
		chip_features |= ATL_HW_CHIP_REVISION_B0 |
			ATL_HW_CHIP_MPI_AQ |
			ATL_HW_CHIP_MIPS |
			ATL_HW_CHIP_TPO2 |
			ATL_HW_CHIP_RPF2;
	} else if ((0xFU & mif_rev) == 0xAU) {
		chip_features |= ATL_HW_CHIP_REVISION_B1 |
			ATL_HW_CHIP_MPI_AQ |
			ATL_HW_CHIP_MIPS |
			ATL_HW_CHIP_TPO2 |
			ATL_HW_CHIP_RPF2;
	}

	*p = chip_features;
}