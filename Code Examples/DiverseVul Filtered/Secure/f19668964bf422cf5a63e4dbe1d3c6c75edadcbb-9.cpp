GF_Err ohdr_box_size(GF_Box *s)
{
	GF_OMADRMCommonHeaderBox *ptr = (GF_OMADRMCommonHeaderBox *)s;
	ptr->size += 1+1+8+2+2+2;
	if (ptr->ContentID) ptr->size += strlen(ptr->ContentID);
	if (ptr->RightsIssuerURL) ptr->size += strlen(ptr->RightsIssuerURL);
	if (ptr->TextualHeadersLen) ptr->size += ptr->TextualHeadersLen;
	return GF_OK;
}