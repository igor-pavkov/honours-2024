static char *nttrans_realloc(char **ptr, size_t size)
{
	if (ptr==NULL) {
		smb_panic("nttrans_realloc() called with NULL ptr");
	}

	*ptr = (char *)SMB_REALLOC(*ptr, size);
	if(*ptr == NULL) {
		return NULL;
	}
	memset(*ptr,'\0',size);
	return *ptr;
}