static int rsi_reset_card(struct rsi_hw *adapter)
{
	int ret;

	rsi_dbg(INFO_ZONE, "Resetting Card...\n");
	rsi_usb_master_reg_write(adapter, RSI_TA_HOLD_REG, 0xE, 4);

	/* This msleep will ensure Thread-Arch processor to go to hold
	 * and any pending dma transfers to rf in device to finish.
	 */
	msleep(100);

	ret = rsi_usb_master_reg_write(adapter, SWBL_REGOUT,
				       RSI_FW_WDT_DISABLE_REQ,
				       RSI_COMMON_REG_SIZE);
	if (ret < 0) {
		rsi_dbg(ERR_ZONE, "Disabling firmware watchdog timer failed\n");
		goto fail;
	}

	if (adapter->device_model != RSI_DEV_9116) {
		ret = usb_ulp_read_write(adapter, RSI_WATCH_DOG_TIMER_1,
					 RSI_ULP_WRITE_2, 32);
		if (ret < 0)
			goto fail;
		ret = usb_ulp_read_write(adapter, RSI_WATCH_DOG_TIMER_2,
					 RSI_ULP_WRITE_0, 32);
		if (ret < 0)
			goto fail;
		ret = usb_ulp_read_write(adapter, RSI_WATCH_DOG_DELAY_TIMER_1,
					 RSI_ULP_WRITE_50, 32);
		if (ret < 0)
			goto fail;
		ret = usb_ulp_read_write(adapter, RSI_WATCH_DOG_DELAY_TIMER_2,
					 RSI_ULP_WRITE_0, 32);
		if (ret < 0)
			goto fail;
		ret = usb_ulp_read_write(adapter, RSI_WATCH_DOG_TIMER_ENABLE,
					 RSI_ULP_TIMER_ENABLE, 32);
		if (ret < 0)
			goto fail;
	} else {
		if ((rsi_usb_master_reg_write(adapter,
					      NWP_WWD_INTERRUPT_TIMER,
					      NWP_WWD_INT_TIMER_CLKS,
					      RSI_9116_REG_SIZE)) < 0) {
			goto fail;
		}
		if ((rsi_usb_master_reg_write(adapter,
					      NWP_WWD_SYSTEM_RESET_TIMER,
					      NWP_WWD_SYS_RESET_TIMER_CLKS,
					      RSI_9116_REG_SIZE)) < 0) {
			goto fail;
		}
		if ((rsi_usb_master_reg_write(adapter,
					      NWP_WWD_MODE_AND_RSTART,
					      NWP_WWD_TIMER_DISABLE,
					      RSI_9116_REG_SIZE)) < 0) {
			goto fail;
		}
	}

	rsi_dbg(INFO_ZONE, "Reset card done\n");
	return ret;

fail:
	rsi_dbg(ERR_ZONE, "Reset card failed\n");
	return ret;
}