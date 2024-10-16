long kvm_arch_vm_ioctl(struct file *filp,
		       unsigned int ioctl, unsigned long arg)
{
	struct kvm *kvm = filp->private_data;
	void __user *argp = (void __user *)arg;
	int r = -ENOTTY;
	/*
	 * This union makes it completely explicit to gcc-3.x
	 * that these two variables' stack usage should be
	 * combined, not added together.
	 */
	union {
		struct kvm_pit_state ps;
		struct kvm_pit_state2 ps2;
		struct kvm_pit_config pit_config;
	} u;

	switch (ioctl) {
	case KVM_SET_TSS_ADDR:
		r = kvm_vm_ioctl_set_tss_addr(kvm, arg);
		break;
	case KVM_SET_IDENTITY_MAP_ADDR: {
		u64 ident_addr;

		mutex_lock(&kvm->lock);
		r = -EINVAL;
		if (kvm->created_vcpus)
			goto set_identity_unlock;
		r = -EFAULT;
		if (copy_from_user(&ident_addr, argp, sizeof(ident_addr)))
			goto set_identity_unlock;
		r = kvm_vm_ioctl_set_identity_map_addr(kvm, ident_addr);
set_identity_unlock:
		mutex_unlock(&kvm->lock);
		break;
	}
	case KVM_SET_NR_MMU_PAGES:
		r = kvm_vm_ioctl_set_nr_mmu_pages(kvm, arg);
		break;
	case KVM_GET_NR_MMU_PAGES:
		r = kvm_vm_ioctl_get_nr_mmu_pages(kvm);
		break;
	case KVM_CREATE_IRQCHIP: {
		mutex_lock(&kvm->lock);

		r = -EEXIST;
		if (irqchip_in_kernel(kvm))
			goto create_irqchip_unlock;

		r = -EINVAL;
		if (kvm->created_vcpus)
			goto create_irqchip_unlock;

		r = kvm_pic_init(kvm);
		if (r)
			goto create_irqchip_unlock;

		r = kvm_ioapic_init(kvm);
		if (r) {
			kvm_pic_destroy(kvm);
			goto create_irqchip_unlock;
		}

		r = kvm_setup_default_irq_routing(kvm);
		if (r) {
			kvm_ioapic_destroy(kvm);
			kvm_pic_destroy(kvm);
			goto create_irqchip_unlock;
		}
		/* Write kvm->irq_routing before enabling irqchip_in_kernel. */
		smp_wmb();
		kvm->arch.irqchip_mode = KVM_IRQCHIP_KERNEL;
	create_irqchip_unlock:
		mutex_unlock(&kvm->lock);
		break;
	}
	case KVM_CREATE_PIT:
		u.pit_config.flags = KVM_PIT_SPEAKER_DUMMY;
		goto create_pit;
	case KVM_CREATE_PIT2:
		r = -EFAULT;
		if (copy_from_user(&u.pit_config, argp,
				   sizeof(struct kvm_pit_config)))
			goto out;
	create_pit:
		mutex_lock(&kvm->lock);
		r = -EEXIST;
		if (kvm->arch.vpit)
			goto create_pit_unlock;
		r = -ENOMEM;
		kvm->arch.vpit = kvm_create_pit(kvm, u.pit_config.flags);
		if (kvm->arch.vpit)
			r = 0;
	create_pit_unlock:
		mutex_unlock(&kvm->lock);
		break;
	case KVM_GET_IRQCHIP: {
		/* 0: PIC master, 1: PIC slave, 2: IOAPIC */
		struct kvm_irqchip *chip;

		chip = memdup_user(argp, sizeof(*chip));
		if (IS_ERR(chip)) {
			r = PTR_ERR(chip);
			goto out;
		}

		r = -ENXIO;
		if (!irqchip_kernel(kvm))
			goto get_irqchip_out;
		r = kvm_vm_ioctl_get_irqchip(kvm, chip);
		if (r)
			goto get_irqchip_out;
		r = -EFAULT;
		if (copy_to_user(argp, chip, sizeof(*chip)))
			goto get_irqchip_out;
		r = 0;
	get_irqchip_out:
		kfree(chip);
		break;
	}
	case KVM_SET_IRQCHIP: {
		/* 0: PIC master, 1: PIC slave, 2: IOAPIC */
		struct kvm_irqchip *chip;

		chip = memdup_user(argp, sizeof(*chip));
		if (IS_ERR(chip)) {
			r = PTR_ERR(chip);
			goto out;
		}

		r = -ENXIO;
		if (!irqchip_kernel(kvm))
			goto set_irqchip_out;
		r = kvm_vm_ioctl_set_irqchip(kvm, chip);
		if (r)
			goto set_irqchip_out;
		r = 0;
	set_irqchip_out:
		kfree(chip);
		break;
	}
	case KVM_GET_PIT: {
		r = -EFAULT;
		if (copy_from_user(&u.ps, argp, sizeof(struct kvm_pit_state)))
			goto out;
		r = -ENXIO;
		if (!kvm->arch.vpit)
			goto out;
		r = kvm_vm_ioctl_get_pit(kvm, &u.ps);
		if (r)
			goto out;
		r = -EFAULT;
		if (copy_to_user(argp, &u.ps, sizeof(struct kvm_pit_state)))
			goto out;
		r = 0;
		break;
	}
	case KVM_SET_PIT: {
		r = -EFAULT;
		if (copy_from_user(&u.ps, argp, sizeof(u.ps)))
			goto out;
		r = -ENXIO;
		if (!kvm->arch.vpit)
			goto out;
		r = kvm_vm_ioctl_set_pit(kvm, &u.ps);
		break;
	}
	case KVM_GET_PIT2: {
		r = -ENXIO;
		if (!kvm->arch.vpit)
			goto out;
		r = kvm_vm_ioctl_get_pit2(kvm, &u.ps2);
		if (r)
			goto out;
		r = -EFAULT;
		if (copy_to_user(argp, &u.ps2, sizeof(u.ps2)))
			goto out;
		r = 0;
		break;
	}
	case KVM_SET_PIT2: {
		r = -EFAULT;
		if (copy_from_user(&u.ps2, argp, sizeof(u.ps2)))
			goto out;
		r = -ENXIO;
		if (!kvm->arch.vpit)
			goto out;
		r = kvm_vm_ioctl_set_pit2(kvm, &u.ps2);
		break;
	}
	case KVM_REINJECT_CONTROL: {
		struct kvm_reinject_control control;
		r =  -EFAULT;
		if (copy_from_user(&control, argp, sizeof(control)))
			goto out;
		r = kvm_vm_ioctl_reinject(kvm, &control);
		break;
	}
	case KVM_SET_BOOT_CPU_ID:
		r = 0;
		mutex_lock(&kvm->lock);
		if (kvm->created_vcpus)
			r = -EBUSY;
		else
			kvm->arch.bsp_vcpu_id = arg;
		mutex_unlock(&kvm->lock);
		break;
	case KVM_XEN_HVM_CONFIG: {
		struct kvm_xen_hvm_config xhc;
		r = -EFAULT;
		if (copy_from_user(&xhc, argp, sizeof(xhc)))
			goto out;
		r = -EINVAL;
		if (xhc.flags)
			goto out;
		memcpy(&kvm->arch.xen_hvm_config, &xhc, sizeof(xhc));
		r = 0;
		break;
	}
	case KVM_SET_CLOCK: {
		struct kvm_clock_data user_ns;
		u64 now_ns;

		r = -EFAULT;
		if (copy_from_user(&user_ns, argp, sizeof(user_ns)))
			goto out;

		r = -EINVAL;
		if (user_ns.flags)
			goto out;

		r = 0;
		/*
		 * TODO: userspace has to take care of races with VCPU_RUN, so
		 * kvm_gen_update_masterclock() can be cut down to locked
		 * pvclock_update_vm_gtod_copy().
		 */
		kvm_gen_update_masterclock(kvm);
		now_ns = get_kvmclock_ns(kvm);
		kvm->arch.kvmclock_offset += user_ns.clock - now_ns;
		kvm_make_all_cpus_request(kvm, KVM_REQ_CLOCK_UPDATE);
		break;
	}
	case KVM_GET_CLOCK: {
		struct kvm_clock_data user_ns;
		u64 now_ns;

		now_ns = get_kvmclock_ns(kvm);
		user_ns.clock = now_ns;
		user_ns.flags = kvm->arch.use_master_clock ? KVM_CLOCK_TSC_STABLE : 0;
		memset(&user_ns.pad, 0, sizeof(user_ns.pad));

		r = -EFAULT;
		if (copy_to_user(argp, &user_ns, sizeof(user_ns)))
			goto out;
		r = 0;
		break;
	}
	case KVM_ENABLE_CAP: {
		struct kvm_enable_cap cap;

		r = -EFAULT;
		if (copy_from_user(&cap, argp, sizeof(cap)))
			goto out;
		r = kvm_vm_ioctl_enable_cap(kvm, &cap);
		break;
	}
	case KVM_MEMORY_ENCRYPT_OP: {
		r = -ENOTTY;
		if (kvm_x86_ops->mem_enc_op)
			r = kvm_x86_ops->mem_enc_op(kvm, argp);
		break;
	}
	case KVM_MEMORY_ENCRYPT_REG_REGION: {
		struct kvm_enc_region region;

		r = -EFAULT;
		if (copy_from_user(&region, argp, sizeof(region)))
			goto out;

		r = -ENOTTY;
		if (kvm_x86_ops->mem_enc_reg_region)
			r = kvm_x86_ops->mem_enc_reg_region(kvm, &region);
		break;
	}
	case KVM_MEMORY_ENCRYPT_UNREG_REGION: {
		struct kvm_enc_region region;

		r = -EFAULT;
		if (copy_from_user(&region, argp, sizeof(region)))
			goto out;

		r = -ENOTTY;
		if (kvm_x86_ops->mem_enc_unreg_region)
			r = kvm_x86_ops->mem_enc_unreg_region(kvm, &region);
		break;
	}
	case KVM_HYPERV_EVENTFD: {
		struct kvm_hyperv_eventfd hvevfd;

		r = -EFAULT;
		if (copy_from_user(&hvevfd, argp, sizeof(hvevfd)))
			goto out;
		r = kvm_vm_ioctl_hv_eventfd(kvm, &hvevfd);
		break;
	}
	default:
		r = -ENOTTY;
	}
out:
	return r;
}