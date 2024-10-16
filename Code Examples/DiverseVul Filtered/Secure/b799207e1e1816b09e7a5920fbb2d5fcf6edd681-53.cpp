static bool bpf_map_ptr_unpriv(const struct bpf_insn_aux_data *aux)
{
	return aux->map_state & BPF_MAP_PTR_UNPRIV;
}