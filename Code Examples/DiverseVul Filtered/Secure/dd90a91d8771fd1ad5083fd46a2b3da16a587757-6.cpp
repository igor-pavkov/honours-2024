static int check_content(CMS_ContentInfo *cms)
{
    ASN1_OCTET_STRING **pos = CMS_get0_content(cms);
    if (!pos || !*pos) {
        CMSerr(CMS_F_CHECK_CONTENT, CMS_R_NO_CONTENT);
        return 0;
    }
    return 1;
}