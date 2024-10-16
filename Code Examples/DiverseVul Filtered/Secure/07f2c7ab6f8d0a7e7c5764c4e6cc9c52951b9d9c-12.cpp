static struct sctp_chunk *sctp_make_asconf_ack(const struct sctp_association *asoc,
					       __u32 serial, int vparam_len)
{
	struct sctp_addiphdr asconf;
	struct sctp_chunk *retval;
	int length = sizeof(asconf) + vparam_len;

	/* Create the chunk.  */
	retval = sctp_make_control(asoc, SCTP_CID_ASCONF_ACK, 0, length,
				   GFP_ATOMIC);
	if (!retval)
		return NULL;

	asconf.serial = htonl(serial);

	retval->subh.addip_hdr =
		sctp_addto_chunk(retval, sizeof(asconf), &asconf);

	return retval;
}