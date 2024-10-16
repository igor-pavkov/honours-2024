_host_id_hash_v2(const guint8 *seed_arr,
                 gsize         seed_len,
                 guint8 *      out_digest /* 32 bytes (NM_UTILS_CHECKSUM_LENGTH_SHA256) */)
{
    nm_auto_free_checksum GChecksum *sum = g_checksum_new(G_CHECKSUM_SHA256);
    const UuidData *                 machine_id_data;
    char                             slen[100];

    /*
        (stat -c '%s' /var/lib/NetworkManager/secret_key;
         echo -n ' ';
         cat /var/lib/NetworkManager/secret_key;
         cat /etc/machine-id | tr -d '\n' | sed -n 's/[a-f0-9-]/\0/pg') | sha256sum
    */

    nm_sprintf_buf(slen, "%" G_GSIZE_FORMAT " ", seed_len);
    g_checksum_update(sum, (const guchar *) slen, strlen(slen));

    g_checksum_update(sum, (const guchar *) seed_arr, seed_len);

    machine_id_data = _machine_id_get(FALSE);
    if (machine_id_data && !machine_id_data->is_fake)
        g_checksum_update(sum, (const guchar *) machine_id_data->str, strlen(machine_id_data->str));

    nm_utils_checksum_get_digest_len(sum, out_digest, NM_UTILS_CHECKSUM_LENGTH_SHA256);
    return out_digest;
}