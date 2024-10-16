static int wanxl_attach(struct net_device *dev, unsigned short encoding,
			unsigned short parity)
{
	port_t *port = dev_to_port(dev);

	if (encoding != ENCODING_NRZ &&
	    encoding != ENCODING_NRZI)
		return -EINVAL;

	if (parity != PARITY_NONE &&
	    parity != PARITY_CRC32_PR1_CCITT &&
	    parity != PARITY_CRC16_PR1_CCITT &&
	    parity != PARITY_CRC32_PR0_CCITT &&
	    parity != PARITY_CRC16_PR0_CCITT)
		return -EINVAL;

	get_status(port)->encoding = encoding;
	get_status(port)->parity = parity;
	return 0;
}