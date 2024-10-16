int cdrom_dummy_generic_packet(struct cdrom_device_info *cdi,
			       struct packet_command *cgc)
{
	if (cgc->sshdr) {
		cgc->sshdr->sense_key = 0x05;
		cgc->sshdr->asc = 0x20;
		cgc->sshdr->ascq = 0x00;
	}

	cgc->stat = -EIO;
	return -EIO;
}