static void seg_desct_to_kvm_desct(struct desc_struct *seg_desc, u16 selector,
				   struct kvm_segment *kvm_desct)
{
	kvm_desct->base = seg_desc->base0;
	kvm_desct->base |= seg_desc->base1 << 16;
	kvm_desct->base |= seg_desc->base2 << 24;
	kvm_desct->limit = seg_desc->limit0;
	kvm_desct->limit |= seg_desc->limit << 16;
	if (seg_desc->g) {
		kvm_desct->limit <<= 12;
		kvm_desct->limit |= 0xfff;
	}
	kvm_desct->selector = selector;
	kvm_desct->type = seg_desc->type;
	kvm_desct->present = seg_desc->p;
	kvm_desct->dpl = seg_desc->dpl;
	kvm_desct->db = seg_desc->d;
	kvm_desct->s = seg_desc->s;
	kvm_desct->l = seg_desc->l;
	kvm_desct->g = seg_desc->g;
	kvm_desct->avl = seg_desc->avl;
	if (!selector)
		kvm_desct->unusable = 1;
	else
		kvm_desct->unusable = 0;
	kvm_desct->padding = 0;
}