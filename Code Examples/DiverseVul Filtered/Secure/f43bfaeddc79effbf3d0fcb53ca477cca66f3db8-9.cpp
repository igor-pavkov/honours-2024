static void atl2_read_pci_cfg(struct atl2_hw *hw, u32 reg, u16 *value)
{
	struct atl2_adapter *adapter = hw->back;
	pci_read_config_word(adapter->pdev, reg, value);
}