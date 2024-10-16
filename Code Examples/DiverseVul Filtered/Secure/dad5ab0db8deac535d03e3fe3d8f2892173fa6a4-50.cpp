void __init arch_reserve_mem_area(acpi_physical_address addr, size_t size)
{
	e820__range_add(addr, size, E820_TYPE_ACPI);
	e820__update_table_print();
}