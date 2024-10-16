BPF_CALL_2(bpf_ringbuf_submit, void *, sample, u64, flags)
{
	bpf_ringbuf_commit(sample, flags, false /* discard */);
	return 0;
}