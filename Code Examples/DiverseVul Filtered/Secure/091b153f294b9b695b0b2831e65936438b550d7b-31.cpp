int read_packet(void *buf, size_t count, struct rx_info *ri)
{
	struct wif *wi = _wi_in; /* XXX */
	int rc;

        rc = wi_read(wi, buf, count, ri);
        if (rc == -1) {
            switch (errno) {
            case EAGAIN:
                    return 0;
            }

            perror("wi_read()");
            return -1;
        }

	return rc;
}