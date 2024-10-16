static int find_boundary(multipart_buffer *self, char *boundary TSRMLS_DC)
{
	char *line;

	/* loop thru lines */
	while( (line = get_line(self TSRMLS_CC)) )
	{
		/* finished if we found the boundary */
		if (!strcmp(line, boundary)) {
			return 1;
		}
	}

	/* didn't find the boundary */
	return 0;
}