static void add_assoc_asn1_string(zval * val, char * key, ASN1_STRING * str) /* {{{ */
{
	add_assoc_stringl(val, key, (char *)str->data, str->length, 1);
}