void ogs_nas_5gs_mobile_identity_guti_to_nas_guti(
        ogs_nas_5gs_mobile_identity_guti_t *mobile_identity_guti,
        ogs_nas_5gs_guti_t *nas_guti)
{
    ogs_assert(mobile_identity_guti);
    ogs_assert(nas_guti);

    memset(nas_guti, 0, sizeof(*nas_guti));

    memcpy(&nas_guti->nas_plmn_id,
            &mobile_identity_guti->nas_plmn_id, OGS_PLMN_ID_LEN);
    memcpy(&nas_guti->amf_id,
            &mobile_identity_guti->amf_id, sizeof(ogs_amf_id_t));
    nas_guti->m_tmsi = be32toh(mobile_identity_guti->m_tmsi);
}