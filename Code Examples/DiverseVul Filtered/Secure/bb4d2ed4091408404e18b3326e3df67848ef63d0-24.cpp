static int ipv6_from_asc(unsigned char *v6, const char *in)
{
    IPV6_STAT v6stat;
    v6stat.total = 0;
    v6stat.zero_pos = -1;
    v6stat.zero_cnt = 0;
    /*
     * Treat the IPv6 representation as a list of values separated by ':'.
     * The presence of a '::' will parse as one, two or three zero length
     * elements.
     */
    if (!CONF_parse_list(in, ':', 0, ipv6_cb, &v6stat))
        return 0;

    /* Now for some sanity checks */

    if (v6stat.zero_pos == -1) {
        /* If no '::' must have exactly 16 bytes */
        if (v6stat.total != 16)
            return 0;
    } else {
        /* If '::' must have less than 16 bytes */
        if (v6stat.total == 16)
            return 0;
        /* More than three zeroes is an error */
        if (v6stat.zero_cnt > 3)
            return 0;
        /* Can only have three zeroes if nothing else present */
        else if (v6stat.zero_cnt == 3) {
            if (v6stat.total > 0)
                return 0;
        }
        /* Can only have two zeroes if at start or end */
        else if (v6stat.zero_cnt == 2) {
            if ((v6stat.zero_pos != 0)
                && (v6stat.zero_pos != v6stat.total))
                return 0;
        } else
            /* Can only have one zero if *not* start or end */
        {
            if ((v6stat.zero_pos == 0)
                || (v6stat.zero_pos == v6stat.total))
                return 0;
        }
    }

    /* Format result */

    if (v6stat.zero_pos >= 0) {
        /* Copy initial part */
        memcpy(v6, v6stat.tmp, v6stat.zero_pos);
        /* Zero middle */
        memset(v6 + v6stat.zero_pos, 0, 16 - v6stat.total);
        /* Copy final part */
        if (v6stat.total != v6stat.zero_pos)
            memcpy(v6 + v6stat.zero_pos + 16 - v6stat.total,
                   v6stat.tmp + v6stat.zero_pos,
                   v6stat.total - v6stat.zero_pos);
    } else
        memcpy(v6, v6stat.tmp, 16);

    return 1;
}