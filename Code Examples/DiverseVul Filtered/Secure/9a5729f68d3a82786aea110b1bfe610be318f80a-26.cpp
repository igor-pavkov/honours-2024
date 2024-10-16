static int sisusb_write_mem_bulk(struct sisusb_usb_data *sisusb, u32 addr,
		char *kernbuffer, int length, const char __user *userbuffer,
		int index, ssize_t *bytes_written)
{
	struct sisusb_packet packet;
	int  ret = 0;
	static int msgcount;
	u8   swap8, fromkern = kernbuffer ? 1 : 0;
	u16  swap16;
	u32  swap32, flag = (length >> 28) & 1;
	char buf[4];

	/* if neither kernbuffer not userbuffer are given, assume
	 * data in obuf
	 */
	if (!fromkern && !userbuffer)
		kernbuffer = sisusb->obuf[index];

	(*bytes_written = 0);

	length &= 0x00ffffff;

	while (length) {
		switch (length) {
		case 1:
			if (userbuffer) {
				if (get_user(swap8, (u8 __user *)userbuffer))
					return -EFAULT;
			} else
				swap8 = kernbuffer[0];

			ret = sisusb_write_memio_byte(sisusb, SISUSB_TYPE_MEM,
					addr, swap8);

			if (!ret)
				(*bytes_written)++;

			return ret;

		case 2:
			if (userbuffer) {
				if (get_user(swap16, (u16 __user *)userbuffer))
					return -EFAULT;
			} else
				swap16 = *((u16 *)kernbuffer);

			ret = sisusb_write_memio_word(sisusb, SISUSB_TYPE_MEM,
					addr, swap16);

			if (!ret)
				(*bytes_written) += 2;

			return ret;

		case 3:
			if (userbuffer) {
				if (copy_from_user(&buf, userbuffer, 3))
					return -EFAULT;
#ifdef __BIG_ENDIAN
				swap32 = (buf[0] << 16) |
					 (buf[1] <<  8) |
					 buf[2];
#else
				swap32 = (buf[2] << 16) |
					 (buf[1] <<  8) |
					 buf[0];
#endif
			} else
#ifdef __BIG_ENDIAN
				swap32 = (kernbuffer[0] << 16) |
					 (kernbuffer[1] <<  8) |
					 kernbuffer[2];
#else
				swap32 = (kernbuffer[2] << 16) |
					 (kernbuffer[1] <<  8) |
					 kernbuffer[0];
#endif

			ret = sisusb_write_memio_24bit(sisusb, SISUSB_TYPE_MEM,
					addr, swap32);

			if (!ret)
				(*bytes_written) += 3;

			return ret;

		case 4:
			if (userbuffer) {
				if (get_user(swap32, (u32 __user *)userbuffer))
					return -EFAULT;
			} else
				swap32 = *((u32 *)kernbuffer);

			ret = sisusb_write_memio_long(sisusb, SISUSB_TYPE_MEM,
					addr, swap32);
			if (!ret)
				(*bytes_written) += 4;

			return ret;

		default:
			if ((length & ~3) > 0x10000) {

				packet.header  = 0x001f;
				packet.address = 0x000001d4;
				packet.data    = addr;
				ret = sisusb_send_bridge_packet(sisusb, 10,
						&packet, 0);
				packet.header  = 0x001f;
				packet.address = 0x000001d0;
				packet.data    = (length & ~3);
				ret |= sisusb_send_bridge_packet(sisusb, 10,
						&packet, 0);
				packet.header  = 0x001f;
				packet.address = 0x000001c0;
				packet.data    = flag | 0x16;
				ret |= sisusb_send_bridge_packet(sisusb, 10,
						&packet, 0);
				if (userbuffer) {
					ret |= sisusb_send_bulk_msg(sisusb,
							SISUSB_EP_GFX_LBULK_OUT,
							(length & ~3),
							NULL, userbuffer, 0,
							bytes_written, 0, 1);
					userbuffer += (*bytes_written);
				} else if (fromkern) {
					ret |= sisusb_send_bulk_msg(sisusb,
							SISUSB_EP_GFX_LBULK_OUT,
							(length & ~3),
							kernbuffer, NULL, 0,
							bytes_written, 0, 1);
					kernbuffer += (*bytes_written);
				} else {
					ret |= sisusb_send_bulk_msg(sisusb,
							SISUSB_EP_GFX_LBULK_OUT,
							(length & ~3),
							NULL, NULL, index,
							bytes_written, 0, 1);
					kernbuffer += ((*bytes_written) &
							(sisusb->obufsize-1));
				}

			} else {

				packet.header  = 0x001f;
				packet.address = 0x00000194;
				packet.data    = addr;
				ret = sisusb_send_bridge_packet(sisusb, 10,
						&packet, 0);
				packet.header  = 0x001f;
				packet.address = 0x00000190;
				packet.data    = (length & ~3);
				ret |= sisusb_send_bridge_packet(sisusb, 10,
						&packet, 0);
				if (sisusb->flagb0 != 0x16) {
					packet.header  = 0x001f;
					packet.address = 0x00000180;
					packet.data    = flag | 0x16;
					ret |= sisusb_send_bridge_packet(sisusb,
							10, &packet, 0);
					sisusb->flagb0 = 0x16;
				}
				if (userbuffer) {
					ret |= sisusb_send_bulk_msg(sisusb,
							SISUSB_EP_GFX_BULK_OUT,
							(length & ~3),
							NULL, userbuffer, 0,
							bytes_written, 0, 1);
					userbuffer += (*bytes_written);
				} else if (fromkern) {
					ret |= sisusb_send_bulk_msg(sisusb,
							SISUSB_EP_GFX_BULK_OUT,
							(length & ~3),
							kernbuffer, NULL, 0,
							bytes_written, 0, 1);
					kernbuffer += (*bytes_written);
				} else {
					ret |= sisusb_send_bulk_msg(sisusb,
							SISUSB_EP_GFX_BULK_OUT,
							(length & ~3),
							NULL, NULL, index,
							bytes_written, 0, 1);
					kernbuffer += ((*bytes_written) &
							(sisusb->obufsize-1));
				}
			}
			if (ret) {
				msgcount++;
				if (msgcount < 500)
					dev_err(&sisusb->sisusb_dev->dev,
							"Wrote %zd of %d bytes, error %d\n",
							*bytes_written, length,
							ret);
				else if (msgcount == 500)
					dev_err(&sisusb->sisusb_dev->dev,
							"Too many errors, logging stopped\n");
			}
			addr += (*bytes_written);
			length -= (*bytes_written);
		}

		if (ret)
			break;

	}

	return ret ? -EIO : 0;
}