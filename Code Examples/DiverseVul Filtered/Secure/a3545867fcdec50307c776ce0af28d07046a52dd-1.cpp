uint8_t* sldns_str2wire_dname(const char* str, size_t* len)
{
	uint8_t dname[LDNS_MAX_DOMAINLEN+1];
	*len = sizeof(dname);
	if(sldns_str2wire_dname_buf(str, dname, len) == 0) {
		uint8_t* r = (uint8_t*)malloc(*len);
		if(r) return memcpy(r, dname, *len);
	}
	*len = 0;
	return NULL;
}