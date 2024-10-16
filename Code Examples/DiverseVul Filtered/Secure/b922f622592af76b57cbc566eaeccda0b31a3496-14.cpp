int hw_atl_utils_initfw(struct aq_hw_s *self, const struct aq_fw_ops **fw_ops)
{
	int err = 0;

	hw_atl_utils_hw_chip_features_init(self,
					   &self->chip_features);

	self->fw_ver_actual = hw_atl_utils_get_fw_version(self);

	if (hw_atl_utils_ver_match(HW_ATL_FW_VER_1X, self->fw_ver_actual)) {
		*fw_ops = &aq_fw_1x_ops;
	} else if (hw_atl_utils_ver_match(HW_ATL_FW_VER_2X, self->fw_ver_actual)) {
		*fw_ops = &aq_fw_2x_ops;
	} else if (hw_atl_utils_ver_match(HW_ATL_FW_VER_3X, self->fw_ver_actual)) {
		*fw_ops = &aq_fw_2x_ops;
	} else if (hw_atl_utils_ver_match(HW_ATL_FW_VER_4X, self->fw_ver_actual)) {
		*fw_ops = &aq_fw_2x_ops;
	} else {
		aq_pr_err("Bad FW version detected: %x\n",
			  self->fw_ver_actual);
		return -EOPNOTSUPP;
	}
	self->aq_fw_ops = *fw_ops;
	err = self->aq_fw_ops->init(self);

	return err;
}