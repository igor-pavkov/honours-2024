static void ql_mac_enable(struct ql3_adapter *qdev, u32 enable)
{
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;
	u32 value;

	if (enable)
		value = (MAC_CONFIG_REG_PE | (MAC_CONFIG_REG_PE << 16));
	else
		value = (MAC_CONFIG_REG_PE << 16);

	if (qdev->mac_index)
		ql_write_page0_reg(qdev, &port_regs->mac1ConfigReg, value);
	else
		ql_write_page0_reg(qdev, &port_regs->mac0ConfigReg, value);
}