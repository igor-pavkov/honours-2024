int dhcp6_option_parse_ia(DHCP6Option *iaoption, DHCP6IA *ia) {
        uint16_t iatype, optlen;
        size_t i, len;
        int r = 0, status;
        uint16_t opt;
        size_t iaaddr_offset;
        uint32_t lt_t1, lt_t2, lt_valid = 0, lt_min = UINT32_MAX;

        assert_return(ia, -EINVAL);
        assert_return(!ia->addresses, -EINVAL);

        iatype = be16toh(iaoption->code);
        len = be16toh(iaoption->len);

        switch (iatype) {
        case SD_DHCP6_OPTION_IA_NA:

                if (len < DHCP6_OPTION_IA_NA_LEN)
                        return -ENOBUFS;

                iaaddr_offset = DHCP6_OPTION_IA_NA_LEN;
                memcpy(&ia->ia_na, iaoption->data, sizeof(ia->ia_na));

                lt_t1 = be32toh(ia->ia_na.lifetime_t1);
                lt_t2 = be32toh(ia->ia_na.lifetime_t2);

                if (lt_t1 && lt_t2 && lt_t1 > lt_t2) {
                        log_dhcp6_client(client, "IA NA T1 %ds > T2 %ds",
                                         lt_t1, lt_t2);
                        return -EINVAL;
                }

                break;

        case SD_DHCP6_OPTION_IA_PD:

                if (len < sizeof(ia->ia_pd))
                        return -ENOBUFS;

                iaaddr_offset = sizeof(ia->ia_pd);
                memcpy(&ia->ia_pd, iaoption->data, sizeof(ia->ia_pd));

                lt_t1 = be32toh(ia->ia_pd.lifetime_t1);
                lt_t2 = be32toh(ia->ia_pd.lifetime_t2);

                if (lt_t1 && lt_t2 && lt_t1 > lt_t2) {
                        log_dhcp6_client(client, "IA PD T1 %ds > T2 %ds",
                                         lt_t1, lt_t2);
                        return -EINVAL;
                }

                break;

        case SD_DHCP6_OPTION_IA_TA:
                if (len < DHCP6_OPTION_IA_TA_LEN)
                        return -ENOBUFS;

                iaaddr_offset = DHCP6_OPTION_IA_TA_LEN;
                memcpy(&ia->ia_ta.id, iaoption->data, sizeof(ia->ia_ta));

                break;

        default:
                return -ENOMSG;
        }

        ia->type = iatype;
        i = iaaddr_offset;

        while (i < len) {
                DHCP6Option *option = (DHCP6Option *)&iaoption->data[i];

                if (len < i + sizeof(*option) || len < i + sizeof(*option) + be16toh(option->len))
                        return -ENOBUFS;

                opt = be16toh(option->code);
                optlen = be16toh(option->len);

                switch (opt) {
                case SD_DHCP6_OPTION_IAADDR:

                        if (!IN_SET(ia->type, SD_DHCP6_OPTION_IA_NA, SD_DHCP6_OPTION_IA_TA)) {
                                log_dhcp6_client(client, "IA Address option not in IA NA or TA option");
                                return -EINVAL;
                        }

                        r = dhcp6_option_parse_address(option, ia, &lt_valid);
                        if (r < 0)
                                return r;

                        if (lt_valid < lt_min)
                                lt_min = lt_valid;

                        break;

                case SD_DHCP6_OPTION_IA_PD_PREFIX:

                        if (!IN_SET(ia->type, SD_DHCP6_OPTION_IA_PD)) {
                                log_dhcp6_client(client, "IA PD Prefix option not in IA PD option");
                                return -EINVAL;
                        }

                        r = dhcp6_option_parse_pdprefix(option, ia, &lt_valid);
                        if (r < 0)
                                return r;

                        if (lt_valid < lt_min)
                                lt_min = lt_valid;

                        break;

                case SD_DHCP6_OPTION_STATUS_CODE:

                        status = dhcp6_option_parse_status(option, optlen + sizeof(DHCP6Option));
                        if (status < 0)
                                return status;
                        if (status > 0) {
                                log_dhcp6_client(client, "IA status %d",
                                                 status);

                                return -EINVAL;
                        }

                        break;

                default:
                        log_dhcp6_client(client, "Unknown IA option %d", opt);
                        break;
                }

                i += sizeof(*option) + optlen;
        }

        switch(iatype) {
        case SD_DHCP6_OPTION_IA_NA:
                if (!ia->ia_na.lifetime_t1 && !ia->ia_na.lifetime_t2) {
                        lt_t1 = lt_min / 2;
                        lt_t2 = lt_min / 10 * 8;
                        ia->ia_na.lifetime_t1 = htobe32(lt_t1);
                        ia->ia_na.lifetime_t2 = htobe32(lt_t2);

                        log_dhcp6_client(client, "Computed IA NA T1 %ds and T2 %ds as both were zero",
                                         lt_t1, lt_t2);
                }

                break;

        case SD_DHCP6_OPTION_IA_PD:
                if (!ia->ia_pd.lifetime_t1 && !ia->ia_pd.lifetime_t2) {
                        lt_t1 = lt_min / 2;
                        lt_t2 = lt_min / 10 * 8;
                        ia->ia_pd.lifetime_t1 = htobe32(lt_t1);
                        ia->ia_pd.lifetime_t2 = htobe32(lt_t2);

                        log_dhcp6_client(client, "Computed IA PD T1 %ds and T2 %ds as both were zero",
                                         lt_t1, lt_t2);
                }

                break;

        default:
                break;
        }

        return 0;
}