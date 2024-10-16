static int fr_dhcp_attr2vp(VALUE_PAIR *vp, const uint8_t *p, size_t alen)
{
	switch (vp->type) {
	case PW_TYPE_BYTE:
		if (alen != 1) goto raw;
		vp->vp_integer = p[0];
		break;
		
	case PW_TYPE_SHORT:
		if (alen != 2) goto raw;
		vp->vp_integer = (p[0] << 8) | p[1];
		break;
		
	case PW_TYPE_INTEGER:
		if (alen != 4) goto raw;
		memcpy(&vp->vp_integer, p, 4);
		vp->vp_integer = ntohl(vp->vp_integer);
		break;
		
	case PW_TYPE_IPADDR:
		if (alen != 4) goto raw;
		memcpy(&vp->vp_ipaddr, p , 4);
		vp->length = 4;
		break;
		
	case PW_TYPE_STRING:
		if (alen > 253) return -1;
		memcpy(vp->vp_strvalue, p , alen);
		vp->vp_strvalue[alen] = '\0';
		break;
		
	raw:
		vp->type = PW_TYPE_OCTETS;

	case PW_TYPE_OCTETS:
		if (alen > 253) return -1;
		memcpy(vp->vp_octets, p, alen);
		break;
		
	case PW_TYPE_TLV:
		return decode_tlv(vp, p, alen);
		
	default:
		fr_strerror_printf("Internal sanity check %d %d", vp->type, __LINE__);
		break;
	} /* switch over type */

	vp->length = alen;
	return 0;
}