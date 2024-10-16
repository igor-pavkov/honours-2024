static char *sql_create_statement(const char *statement, const char *prop,
				  const char *user, const char *realm, 
				  const char *value,  
				  const sasl_utils_t *utils)
{
    const char *ptr, *line_ptr;
    char *buf, *buf_ptr;
    int filtersize;
    int ulen, plen, rlen, vlen;
    int numpercents=0;
    int biggest;
    size_t i;
    
    /* calculate memory needed for creating the complete query string. */
    ulen = (int)strlen(user);
    rlen = (int)strlen(realm);
    plen = (int)strlen(prop);
    vlen = (int)sql_len(value);
    
    /* what if we have multiple %foo occurrences in the input query? */
    for (i = 0; i < strlen(statement); i++) {
	if (statement[i] == '%') {
	    numpercents++;
	}
    }
    
    /* find the biggest of ulen, rlen, plen, vlen */
    biggest = sql_max(sql_max(ulen, rlen), sql_max(plen, vlen));
    
    /* plus one for the semicolon...and don't forget the trailing 0x0 */
    filtersize = (int)strlen(statement) + 1 + (numpercents*biggest)+1;
    
    /* ok, now try to allocate a chunk of that size */
    buf = (char *) utils->malloc(filtersize);
    
    if (!buf) {
	MEMERROR(utils);
	return NULL;
    }
    
    buf_ptr = buf;
    line_ptr = statement;
    
    /* replace the strings */
    while ( (ptr = strchr(line_ptr, '%')) ) {
	/* copy up to but not including the next % */
	memcpy(buf_ptr, line_ptr, ptr - line_ptr); 
	buf_ptr += ptr - line_ptr;
	ptr++;
	switch (ptr[0]) {
	case '%':
	    buf_ptr[0] = '%';
	    buf_ptr++;
	    break;
	case 'u':
	    memcpy(buf_ptr, user, ulen);
	    buf_ptr += ulen;
	    break;
	case 'r':
	    memcpy(buf_ptr, realm, rlen);
	    buf_ptr += rlen;
	    break;
	case 'p':
	    memcpy(buf_ptr, prop, plen);
	    buf_ptr += plen;
	    break;
	case 'v':
	    if (value != NULL) {
		memcpy(buf_ptr, value, vlen);
		buf_ptr += vlen;
	    }
	    else {
		utils->log(utils->conn, SASL_LOG_ERR,
			   "'%%v' shouldn't be in a SELECT or DELETE");
	    }
	    break;
	default:
	    buf_ptr[0] = '%';
	    buf_ptr[1] = ptr[0];
	    buf_ptr += 2;
	    break;
	}
	ptr++;
	line_ptr = ptr;
    }
    
    memcpy(buf_ptr, line_ptr, strlen(line_ptr)+1);
    /* Make sure the current portion of the statement ends with a semicolon */
    if (buf_ptr[strlen(buf_ptr-1)] != ';') {
	strcat(buf_ptr, ";");
    }

    return (buf);
}