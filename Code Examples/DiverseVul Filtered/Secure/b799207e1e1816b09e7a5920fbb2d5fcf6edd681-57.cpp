static bool arg_type_is_mem_ptr(enum bpf_arg_type type)
{
	return type == ARG_PTR_TO_MEM ||
	       type == ARG_PTR_TO_MEM_OR_NULL ||
	       type == ARG_PTR_TO_UNINIT_MEM;
}