u32 megasas_readl(struct megasas_instance *instance,
		  const volatile void __iomem *addr)
{
	u32 i = 0, ret_val;
	/*
	 * Due to a HW errata in Aero controllers, reads to certain
	 * Fusion registers could intermittently return all zeroes.
	 * This behavior is transient in nature and subsequent reads will
	 * return valid value. As a workaround in driver, retry readl for
	 * upto three times until a non-zero value is read.
	 */
	if (instance->adapter_type == AERO_SERIES) {
		do {
			ret_val = readl(addr);
			i++;
		} while (ret_val == 0 && i < 3);
		return ret_val;
	} else {
		return readl(addr);
	}
}