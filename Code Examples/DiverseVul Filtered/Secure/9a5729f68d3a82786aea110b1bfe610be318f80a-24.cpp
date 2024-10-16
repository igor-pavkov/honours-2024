static int sisusb_read_mem_bulk(struct sisusb_usb_data *sisusb, u32 addr,
		char *kernbuffer, int length, char __user *userbuffer,
		ssize_t *bytes_read)
{
	int ret = 0;
	char buf[4];
	u16 swap16;
	u32 swap32;

	(*bytes_read = 0);

	length &= 0x00ffffff;

	while (length) {
		switch (length) {
		case 1:
			ret |= sisusb_read_memio_byte(sisusb, SISUSB_TYPE_MEM,
					addr, &buf[0]);
			if (!ret) {
				(*bytes_read)++;
				if (userbuffer) {
					if (put_user(buf[0], (u8 __user *)userbuffer))
						return -EFAULT;
				} else
					kernbuffer[0] = buf[0];
			}
			return ret;

		case 2:
			ret |= sisusb_read_memio_word(sisusb, SISUSB_TYPE_MEM,
					addr, &swap16);
			if (!ret) {
				(*bytes_read) += 2;
				if (userbuffer) {
					if (put_user(swap16, (u16 __user *)userbuffer))
						return -EFAULT;
				} else {
					*((u16 *)kernbuffer) = swap16;
				}
			}
			return ret;

		case 3:
			ret |= sisusb_read_memio_24bit(sisusb, SISUSB_TYPE_MEM,
					addr, &swap32);
			if (!ret) {
				(*bytes_read) += 3;
#ifdef __BIG_ENDIAN
				buf[0] = (swap32 >> 16) & 0xff;
				buf[1] = (swap32 >> 8) & 0xff;
				buf[2] = swap32 & 0xff;
#else
				buf[2] = (swap32 >> 16) & 0xff;
				buf[1] = (swap32 >> 8) & 0xff;
				buf[0] = swap32 & 0xff;
#endif
				if (userbuffer) {
					if (copy_to_user(userbuffer,
							&buf[0], 3))
						return -EFAULT;
				} else {
					kernbuffer[0] = buf[0];
					kernbuffer[1] = buf[1];
					kernbuffer[2] = buf[2];
				}
			}
			return ret;

		default:
			ret |= sisusb_read_memio_long(sisusb, SISUSB_TYPE_MEM,
					addr, &swap32);
			if (!ret) {
				(*bytes_read) += 4;
				if (userbuffer) {
					if (put_user(swap32, (u32 __user *)userbuffer))
						return -EFAULT;

					userbuffer += 4;
				} else {
					*((u32 *)kernbuffer) = swap32;
					kernbuffer += 4;
				}
				addr += 4;
				length -= 4;
			}
		}
		if (ret)
			break;
	}

	return ret;
}