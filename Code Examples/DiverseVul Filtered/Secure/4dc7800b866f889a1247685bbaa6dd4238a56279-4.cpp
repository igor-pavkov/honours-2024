static size_t fr_dhcp_vp2attr(VALUE_PAIR *vp, uint8_t *p, size_t room)
{
	size_t length;
	uint32_t lvalue;

	/*
	 *	FIXME: Check room!
	 */
	room = room;		/* -Wunused */

	/*
	 *	Search for all attributes of the same
	 *	type, and pack them into the same
	 *	attribute.
	 */
	switch (vp->type) {
	case PW_TYPE_BYTE:
		length = 1;
		*p = vp->vp_integer & 0xff;
		break;
		
	case PW_TYPE_SHORT:
		length = 2;
		p[0] = (vp->vp_integer >> 8) & 0xff;
		p[1] = vp->vp_integer & 0xff;
		break;
		
	case PW_TYPE_INTEGER:
		length = 4;
		lvalue = htonl(vp->vp_integer);
		memcpy(p, &lvalue, 4);
		break;
		
	case PW_TYPE_IPADDR:
		length = 4;
		memcpy(p, &vp->vp_ipaddr, 4);
		break;
		
	case PW_TYPE_ETHERNET:
		length = 6;
		memcpy(p, &vp->vp_ether, 6);
		break;
		
	case PW_TYPE_STRING:
		memcpy(p, vp->vp_strvalue, vp->length);
		length = vp->length;
		break;
		
	case PW_TYPE_TLV:	/* FIXME: split it on 255? */
		memcpy(p, vp->vp_tlv, vp->length);
		length = vp->length;
		break;

	case PW_TYPE_OCTETS:
		memcpy(p, vp->vp_octets, vp->length);
		length = vp->length;
		break;
		
	default:
		fr_strerror_printf("BAD TYPE2 %d", vp->type);
		length = 0;
		break;
	}

	return length;
}