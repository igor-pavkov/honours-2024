static int option_parse_hdr(uint8_t **buf, size_t *buflen, uint16_t *optcode, size_t *optlen) {
        DHCP6Option *option = (DHCP6Option*) *buf;
        uint16_t len;

        assert_return(buf, -EINVAL);
        assert_return(optcode, -EINVAL);
        assert_return(optlen, -EINVAL);

        if (*buflen < sizeof(DHCP6Option))
                return -ENOMSG;

        len = be16toh(option->len);

        if (len > *buflen)
                return -ENOMSG;

        *optcode = be16toh(option->code);
        *optlen = len;

        *buf += 4;
        *buflen -= 4;

        return 0;
}