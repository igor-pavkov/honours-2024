bool r_pkcs7_parse_signerinfos (RPKCS7SignerInfos *ss, RASN1Object *object) {
	ut32 i;
	if (!ss && !object) {
		return false;
	}
	if (object->list.length > 0) {
		ss->elements = (RPKCS7SignerInfo **) calloc (object->list.length, sizeof (RPKCS7SignerInfo*));
		if (!ss->elements) {
			return false;
		}
		ss->length = object->list.length;
		for (i = 0; i < ss->length; ++i) {
			// r_pkcs7_parse_signerinfo returns bool,
			// so i have to allocate before calling the function
			ss->elements[i] = R_NEW0 (RPKCS7SignerInfo);
			//should i handle invalid memory? the function checks the pointer
			//or it should return if si->elements[i] == NULL ?
			r_pkcs7_parse_signerinfo (ss->elements[i], object->list.objects[i]);
		}
	}
	return true;
}