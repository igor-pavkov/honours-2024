void kvm_enable_efer_bits(u64 mask)
{
       efer_reserved_bits &= ~mask;
}