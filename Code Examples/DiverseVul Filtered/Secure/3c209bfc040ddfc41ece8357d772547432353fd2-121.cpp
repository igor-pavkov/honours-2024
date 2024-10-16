void Item_subselect::get_cache_parameters(List<Item> &parameters)
{
  Collect_deps_prm prm= {&parameters,      // parameters
    unit->first_select()->nest_level_base, // nest_level_base
    0,                                     // count
    unit->first_select()->nest_level,      // nest_level
    TRUE                                   // collect
  };
  walk(&Item::collect_outer_ref_processor, TRUE, &prm);
}