static void apply_header_digest(struct iscsi_context *iscsi, QemuOpts *opts,
                                Error **errp)
{
    const char *digest = NULL;

    digest = qemu_opt_get(opts, "header-digest");
    if (!digest) {
        iscsi_set_header_digest(iscsi, ISCSI_HEADER_DIGEST_NONE_CRC32C);
    } else if (!strcmp(digest, "crc32c")) {
        iscsi_set_header_digest(iscsi, ISCSI_HEADER_DIGEST_CRC32C);
    } else if (!strcmp(digest, "none")) {
        iscsi_set_header_digest(iscsi, ISCSI_HEADER_DIGEST_NONE);
    } else if (!strcmp(digest, "crc32c-none")) {
        iscsi_set_header_digest(iscsi, ISCSI_HEADER_DIGEST_CRC32C_NONE);
    } else if (!strcmp(digest, "none-crc32c")) {
        iscsi_set_header_digest(iscsi, ISCSI_HEADER_DIGEST_NONE_CRC32C);
    } else {
        error_setg(errp, "Invalid header-digest setting : %s", digest);
    }
}