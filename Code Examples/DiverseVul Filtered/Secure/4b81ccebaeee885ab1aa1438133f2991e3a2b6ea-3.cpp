BPF_CALL_2(bpf_ringbuf_discard, void *, sample, u64, flags)
{
	bpf_ringbuf_commit(sample, flags, true /* discard */);
	return 0;
}