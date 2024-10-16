static Array zipStatToArray(struct zip_stat* zipStat) {
  if (zipStat == nullptr) {
    return Array();
  }

  return make_map_array(
    s_name,        String(zipStat->name),
    s_index,       VarNR(zipStat->index),
    s_crc,         VarNR(static_cast<int64_t>(zipStat->crc)),
    s_size,        VarNR(zipStat->size),
    s_mtime,       VarNR(zipStat->mtime),
    s_comp_size,   VarNR(zipStat->comp_size),
    s_comp_method, VarNR(zipStat->comp_method)
  );
}