static char *resolve_priorities(const char* priorities)
{
char *p = (char*)priorities;
char *additional = NULL;
char *ret = NULL;
char *ss, *line = NULL;
unsigned ss_len;
int l;
FILE* fp = NULL;
size_t n, n2 = 0, line_size;

	while (c_isspace(*p))
		p++;

	if (*p == '@') {
		ss = p+1;

		additional = strchr(p, ':');
		if (additional != NULL) {
			ss_len = additional - ss;
			additional++;
		} else {
			ss_len = strlen(ss);
		}

		fp = fopen(SYSTEM_PRIORITY_FILE, "r");
		if (fp == NULL) {/* fail */
			ret = NULL;
			goto finish;
		}

		do {
			l = getline(&line, &line_size, fp);
			if (l > 0) {
				p = check_str(line, line_size, ss, ss_len);
				if (p != NULL)
					break;
			}
		} while (l>0);

		if (p == NULL) {
			ret = NULL;
			goto finish;
		}

		n = strlen(p);
		if (additional)
			n2 = strlen(additional);

		ret = malloc(n+n2+1+1);
		if (ret == NULL) {
			goto finish;
		}

		memcpy(ret, p, n);
		if (additional != NULL) {
			ret[n] = ':';
			memcpy(&ret[n+1], additional, n2);
			ret[n+n2+1] = 0;
		} else {
			ret[n] = 0;
		}
	} else {
		return strdup(p);
	}

finish:
	if (ret != NULL) {
		_gnutls_debug_log("selected priority string: %s\n", ret);
	}
	free(line);
	if (fp != NULL)
		fclose(fp);

	return ret;
}