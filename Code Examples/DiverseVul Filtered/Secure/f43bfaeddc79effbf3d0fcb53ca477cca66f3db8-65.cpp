static int get_permanent_address(struct atl2_hw *hw)
{
	u32 Addr[2];
	u32 i, Control;
	u16 Register;
	u8  EthAddr[ETH_ALEN];
	bool KeyValid;

	if (is_valid_ether_addr(hw->perm_mac_addr))
		return 0;

	Addr[0] = 0;
	Addr[1] = 0;

	if (!atl2_check_eeprom_exist(hw)) { /* eeprom exists */
		Register = 0;
		KeyValid = false;

		/* Read out all EEPROM content */
		i = 0;
		while (1) {
			if (atl2_read_eeprom(hw, i + 0x100, &Control)) {
				if (KeyValid) {
					if (Register == REG_MAC_STA_ADDR)
						Addr[0] = Control;
					else if (Register ==
						(REG_MAC_STA_ADDR + 4))
						Addr[1] = Control;
					KeyValid = false;
				} else if ((Control & 0xff) == 0x5A) {
					KeyValid = true;
					Register = (u16) (Control >> 16);
				} else {
			/* assume data end while encount an invalid KEYWORD */
					break;
				}
			} else {
				break; /* read error */
			}
			i += 4;
		}

		*(u32 *) &EthAddr[2] = LONGSWAP(Addr[0]);
		*(u16 *) &EthAddr[0] = SHORTSWAP(*(u16 *) &Addr[1]);

		if (is_valid_ether_addr(EthAddr)) {
			memcpy(hw->perm_mac_addr, EthAddr, ETH_ALEN);
			return 0;
		}
		return 1;
	}

	/* see if SPI flash exists? */
	Addr[0] = 0;
	Addr[1] = 0;
	Register = 0;
	KeyValid = false;
	i = 0;
	while (1) {
		if (atl2_spi_read(hw, i + 0x1f000, &Control)) {
			if (KeyValid) {
				if (Register == REG_MAC_STA_ADDR)
					Addr[0] = Control;
				else if (Register == (REG_MAC_STA_ADDR + 4))
					Addr[1] = Control;
				KeyValid = false;
			} else if ((Control & 0xff) == 0x5A) {
				KeyValid = true;
				Register = (u16) (Control >> 16);
			} else {
				break; /* data end */
			}
		} else {
			break; /* read error */
		}
		i += 4;
	}

	*(u32 *) &EthAddr[2] = LONGSWAP(Addr[0]);
	*(u16 *) &EthAddr[0] = SHORTSWAP(*(u16 *)&Addr[1]);
	if (is_valid_ether_addr(EthAddr)) {
		memcpy(hw->perm_mac_addr, EthAddr, ETH_ALEN);
		return 0;
	}
	/* maybe MAC-address is from BIOS */
	Addr[0] = ATL2_READ_REG(hw, REG_MAC_STA_ADDR);
	Addr[1] = ATL2_READ_REG(hw, REG_MAC_STA_ADDR + 4);
	*(u32 *) &EthAddr[2] = LONGSWAP(Addr[0]);
	*(u16 *) &EthAddr[0] = SHORTSWAP(*(u16 *) &Addr[1]);

	if (is_valid_ether_addr(EthAddr)) {
		memcpy(hw->perm_mac_addr, EthAddr, ETH_ALEN);
		return 0;
	}

	return 1;
}