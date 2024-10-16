static int kvm_vcpu_ioctl_interrupt(struct kvm_vcpu *vcpu,
				    struct kvm_interrupt *irq)
{
	if (irq->irq < 0 || irq->irq >= 256)
		return -EINVAL;
	if (irqchip_in_kernel(vcpu->kvm))
		return -ENXIO;
	vcpu_load(vcpu);

	set_bit(irq->irq, vcpu->arch.irq_pending);
	set_bit(irq->irq / BITS_PER_LONG, &vcpu->arch.irq_summary);

	vcpu_put(vcpu);

	return 0;
}