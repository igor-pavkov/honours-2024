static int rsi_usb_load_data_master_write(struct rsi_hw *adapter,
					  u32 base_address,
					  u32 instructions_sz, u16 block_size,
					  u8 *ta_firmware)
{
	u16 num_blocks;
	u32 cur_indx, i;
	u8 temp_buf[256];
	int status;

	num_blocks = instructions_sz / block_size;
	rsi_dbg(INFO_ZONE, "num_blocks: %d\n", num_blocks);

	for (cur_indx = 0, i = 0; i < num_blocks; i++, cur_indx += block_size) {
		memcpy(temp_buf, ta_firmware + cur_indx, block_size);
		status = rsi_usb_write_register_multiple(adapter, base_address,
							 (u8 *)(temp_buf),
							 block_size);
		if (status < 0)
			return status;

		rsi_dbg(INFO_ZONE, "%s: loading block: %d\n", __func__, i);
		base_address += block_size;
	}

	if (instructions_sz % block_size) {
		memset(temp_buf, 0, block_size);
		memcpy(temp_buf, ta_firmware + cur_indx,
		       instructions_sz % block_size);
		status = rsi_usb_write_register_multiple
						(adapter, base_address,
						 (u8 *)temp_buf,
						 instructions_sz % block_size);
		if (status < 0)
			return status;
		rsi_dbg(INFO_ZONE,
			"Written Last Block in Address 0x%x Successfully\n",
			cur_indx);
	}
	return 0;
}