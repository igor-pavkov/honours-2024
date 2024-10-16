  pass_ipa_cp (gcc::context *ctxt)
    : ipa_opt_pass_d (pass_data_ipa_cp, ctxt,
		      ipcp_generate_summary, /* generate_summary */
		      ipcp_write_summary, /* write_summary */
		      ipcp_read_summary, /* read_summary */
		      ipcp_write_transformation_summaries, /*
		      write_optimization_summary */
		      ipcp_read_transformation_summaries, /*
		      read_optimization_summary */
		      NULL, /* stmt_fixup */
		      0, /* function_transform_todo_flags_start */
		      ipcp_transform_function, /* function_transform */
		      NULL) /* variable_transform */
  {}