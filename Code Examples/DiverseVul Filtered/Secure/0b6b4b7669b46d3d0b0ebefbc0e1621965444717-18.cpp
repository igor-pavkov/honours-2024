errno_t get_sysdb_grouplist(TALLOC_CTX *mem_ctx,
                            struct sysdb_ctx *sysdb,
                            struct sss_domain_info *domain,
                            const char *name,
                            char ***grouplist)
{
    return get_sysdb_grouplist_ex(mem_ctx, sysdb, domain,
                                  name, grouplist, false);
}