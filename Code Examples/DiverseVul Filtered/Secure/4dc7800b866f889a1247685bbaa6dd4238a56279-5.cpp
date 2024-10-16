static int decode_tlv(VALUE_PAIR *tlv, const uint8_t *data, size_t data_len)
{
	const uint8_t *p;
	VALUE_PAIR *head, **tail, *vp;

	/*
	 *	Take a pass at parsing it.
	 */
	p = data;
	while (p < (data + data_len)) {
		if ((p + 2) > (data + data_len)) goto make_tlv;

		if ((p + p[1] + 2) > (data + data_len)) goto make_tlv;
		p += 2 + p[1];
	}

	/*
	 *	Got here... must be well formed.
	 */
	head = NULL;
	tail = &head;

	p = data;
	while (p < (data + data_len)) {
		vp = paircreate(tlv->attribute | (p[0] << 8), PW_TYPE_OCTETS);
		if (!vp) {
			pairfree(&head);
			goto make_tlv;
		}

		if (fr_dhcp_attr2vp(vp, p + 2, p[1]) < 0) {
			pairfree(&head);
			goto make_tlv;
		}

		*tail = vp;
		tail = &(vp->next);
		p += 2 + p[1];
	}

	/*
	 *	The caller allocated TLV, so we need to copy the FIRST
	 *	attribute over top of that.
	 */
	memcpy(tlv, head, sizeof(*tlv));
	head->next = NULL;
	pairfree(&head);
	return 0;

make_tlv:
	tlv->vp_tlv = malloc(data_len);
	if (!tlv->vp_tlv) {
		fr_strerror_printf("No memory");
		return -1;
	}
	memcpy(tlv->vp_tlv, data, data_len);
	tlv->length = data_len;
	
	return 0;
}