static VALUE_PAIR *fr_dhcp_vp2suboption(VALUE_PAIR *vps)
{
	int length;
	unsigned int attribute;
	uint8_t *ptr;
	VALUE_PAIR *vp, *tlv;

	attribute = vps->attribute & 0xffff00ff;

	tlv = paircreate(attribute, PW_TYPE_TLV);
	if (!tlv) return NULL;

	tlv->length = 0;
	for (vp = vps; vp != NULL; vp = vp->next) {
		/*
		 *	Group the attributes ONLY until we see a
		 *	non-TLV attribute.
		 */
		if (!vp->flags.is_tlv ||
		    vp->flags.encoded ||
		    ((vp->attribute & 0xffff00ff) != attribute)) {
			break;
		}

		tlv->length += vp->length + 2;
	}

	if (!tlv->length) {
		pairfree(&tlv);
		return NULL;
	}

	tlv->vp_tlv = malloc(tlv->length);
	if (!tlv->vp_tlv) {
		pairfree(&tlv);
		return NULL;
	}

	ptr = tlv->vp_tlv;
	for (vp = vps; vp != NULL; vp = vp->next) {
		if (!vp->flags.is_tlv ||
		    vp->flags.encoded ||
		    ((vp->attribute & 0xffff00ff) != attribute)) {
			break;
		}

		length = fr_dhcp_vp2attr(vp, ptr + 2,
					 tlv->vp_tlv + tlv->length - ptr);
		if (length > 255) return NULL;

		/*
		 *	Pack the attribute.
		 */
		ptr[0] = (vp->attribute & 0xff00) >> 8;
		ptr[1] = length;

		ptr += length + 2;
		vp->flags.encoded = 1;
	}

	return tlv;
}