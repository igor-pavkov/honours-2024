const char *missing_required_preauth(krb5_db_entry *client,
                                     krb5_db_entry *server,
                                     krb5_enc_tkt_part *enc_tkt_reply)
{
#ifdef DEBUG
    krb5_klog_syslog (
        LOG_DEBUG,
        "client needs %spreauth, %shw preauth; request has %spreauth, %shw preauth",
        isflagset (client->attributes, KRB5_KDB_REQUIRES_PRE_AUTH) ? "" : "no ",
        isflagset (client->attributes, KRB5_KDB_REQUIRES_HW_AUTH) ? "" : "no ",
        isflagset (enc_tkt_reply->flags, TKT_FLG_PRE_AUTH) ? "" : "no ",
        isflagset (enc_tkt_reply->flags, TKT_FLG_HW_AUTH) ? "" : "no ");
#endif

    if (isflagset(client->attributes, KRB5_KDB_REQUIRES_PRE_AUTH) &&
        !isflagset(enc_tkt_reply->flags, TKT_FLG_PRE_AUTH))
        return "NEEDED_PREAUTH";

    if (isflagset(client->attributes, KRB5_KDB_REQUIRES_HW_AUTH) &&
        !isflagset(enc_tkt_reply->flags, TKT_FLG_HW_AUTH))
        return "NEEDED_HW_PREAUTH";

    return 0;
}