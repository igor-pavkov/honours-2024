static void __exit vmx_exit(void)
{
	__free_page(vmx_msr_bitmap);
	__free_page(vmx_io_bitmap_b);
	__free_page(vmx_io_bitmap_a);

	kvm_exit();
}