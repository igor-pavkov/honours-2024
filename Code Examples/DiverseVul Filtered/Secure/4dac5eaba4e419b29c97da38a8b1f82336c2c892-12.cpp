static int option_append_hdr(uint8_t **buf, size_t *buflen, uint16_t optcode,
                             size_t optlen) {
        DHCP6Option *option = (DHCP6Option*) *buf;

        assert_return(buf, -EINVAL);
        assert_return(*buf, -EINVAL);
        assert_return(buflen, -EINVAL);

        if (optlen > 0xffff || *buflen < optlen + sizeof(DHCP6Option))
                return -ENOBUFS;

        option->code = htobe16(optcode);
        option->len = htobe16(optlen);

        *buf += sizeof(DHCP6Option);
        *buflen -= sizeof(DHCP6Option);

        return 0;
}