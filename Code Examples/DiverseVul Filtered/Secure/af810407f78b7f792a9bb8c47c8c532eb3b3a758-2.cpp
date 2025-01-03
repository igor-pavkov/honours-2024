Alter_inplace_info::Alter_inplace_info(HA_CREATE_INFO *create_info_arg,
                     Alter_info *alter_info_arg,
                     KEY *key_info_arg, uint key_count_arg,
                     partition_info *modified_part_info_arg,
                     bool ignore_arg, bool error_non_empty)
    : create_info(create_info_arg),
    alter_info(alter_info_arg),
    key_info_buffer(key_info_arg),
    key_count(key_count_arg),
    index_drop_count(0),
    index_drop_buffer(nullptr),
    index_add_count(0),
    index_add_buffer(nullptr),
    rename_keys(current_thd->mem_root),
    handler_ctx(nullptr),
    group_commit_ctx(nullptr),
    handler_flags(0),
    modified_part_info(modified_part_info_arg),
    ignore(ignore_arg),
    online(false),
    unsupported_reason(nullptr),
    error_if_not_empty(error_non_empty)
  {}