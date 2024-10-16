void ogs_nas_5gs_imsi_to_bcd(
    ogs_nas_5gs_mobile_identity_t *mobile_identity, char *imsi_bcd)
{
    ogs_nas_5gs_mobile_identity_suci_t *mobile_identity_suci = NULL;
    ogs_plmn_id_t plmn_id;
    char tmp[OGS_MAX_IMSI_BCD_LEN+1];
    char *p, *last;

    ogs_assert(mobile_identity);
    ogs_assert(imsi_bcd);

    p = imsi_bcd;
    last = imsi_bcd + OGS_MAX_IMSI_BCD_LEN + 1;

    mobile_identity_suci =
        (ogs_nas_5gs_mobile_identity_suci_t *)mobile_identity->buffer;
    ogs_assert(mobile_identity_suci);

    ogs_nas_to_plmn_id(&plmn_id, &mobile_identity_suci->nas_plmn_id);
    if (ogs_plmn_id_mnc_len(&plmn_id) == 2)
        p = ogs_slprintf(p, last, "%03d%02d",
                ogs_plmn_id_mcc(&plmn_id), ogs_plmn_id_mnc(&plmn_id));
    else 
        p = ogs_slprintf(p, last, "%03d%03d",
                ogs_plmn_id_mcc(&plmn_id), ogs_plmn_id_mnc(&plmn_id));

    ogs_assert(mobile_identity->length > 8);
    ogs_buffer_to_bcd(mobile_identity_suci->scheme_output,
            mobile_identity->length - 8, tmp);
    p = ogs_slprintf(p, last, "%s", tmp);
}