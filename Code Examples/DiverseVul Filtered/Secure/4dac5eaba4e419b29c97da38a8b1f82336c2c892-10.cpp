int dhcp6_option_parse_domainname(const uint8_t *optval, uint16_t optlen, char ***str_arr) {
        size_t pos = 0, idx = 0;
        _cleanup_strv_free_ char **names = NULL;
        int r;

        assert_return(optlen > 1, -ENODATA);
        assert_return(optval[optlen - 1] == '\0', -EINVAL);

        while (pos < optlen) {
                _cleanup_free_ char *ret = NULL;
                size_t n = 0, allocated = 0;
                bool first = true;

                for (;;) {
                        const char *label;
                        uint8_t c;

                        c = optval[pos++];

                        if (c == 0)
                                /* End of name */
                                break;
                        if (c > 63)
                                return -EBADMSG;

                        /* Literal label */
                        label = (const char *)&optval[pos];
                        pos += c;
                        if (pos >= optlen)
                                return -EMSGSIZE;

                        if (!GREEDY_REALLOC(ret, allocated, n + !first + DNS_LABEL_ESCAPED_MAX))
                                return -ENOMEM;

                        if (first)
                                first = false;
                        else
                                ret[n++] = '.';

                        r = dns_label_escape(label, c, ret + n, DNS_LABEL_ESCAPED_MAX);
                        if (r < 0)
                                return r;

                        n += r;
                }

                if (n == 0)
                        continue;

                if (!GREEDY_REALLOC(ret, allocated, n + 1))
                        return -ENOMEM;

                ret[n] = 0;

                r = strv_extend(&names, ret);
                if (r < 0)
                        return r;

                idx++;
        }

        *str_arr = TAKE_PTR(names);

        return idx;
}