int dhcp6_option_append_pd(uint8_t *buf, size_t len, const DHCP6IA *pd) {
        DHCP6Option *option = (DHCP6Option *)buf;
        size_t i = sizeof(*option) + sizeof(pd->ia_pd);
        DHCP6Address *prefix;

        assert_return(buf, -EINVAL);
        assert_return(pd, -EINVAL);
        assert_return(pd->type == SD_DHCP6_OPTION_IA_PD, -EINVAL);

        if (len < i)
                return -ENOBUFS;

        option->code = htobe16(SD_DHCP6_OPTION_IA_PD);

        memcpy(&option->data, &pd->ia_pd, sizeof(pd->ia_pd));

        LIST_FOREACH(addresses, prefix, pd->addresses) {
                DHCP6PDPrefixOption *prefix_opt;

                if (len < i + sizeof(*prefix_opt))
                        return -ENOBUFS;

                prefix_opt = (DHCP6PDPrefixOption *)&buf[i];
                prefix_opt->option.code = htobe16(SD_DHCP6_OPTION_IA_PD_PREFIX);
                prefix_opt->option.len = htobe16(sizeof(prefix_opt->iapdprefix));

                memcpy(&prefix_opt->iapdprefix, &prefix->iapdprefix,
                       sizeof(struct iapdprefix));

                i += sizeof(*prefix_opt);
        }

        option->len = htobe16(i - sizeof(*option));

        return i;
}