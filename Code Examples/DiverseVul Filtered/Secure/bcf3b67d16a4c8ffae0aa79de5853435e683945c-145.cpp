static inline void megasas_set_adapter_type(struct megasas_instance *instance)
{
	if ((instance->pdev->vendor == PCI_VENDOR_ID_DELL) &&
	    (instance->pdev->device == PCI_DEVICE_ID_DELL_PERC5)) {
		instance->adapter_type = MFI_SERIES;
	} else {
		switch (instance->pdev->device) {
		case PCI_DEVICE_ID_LSI_AERO_10E1:
		case PCI_DEVICE_ID_LSI_AERO_10E2:
		case PCI_DEVICE_ID_LSI_AERO_10E5:
		case PCI_DEVICE_ID_LSI_AERO_10E6:
			instance->adapter_type = AERO_SERIES;
			break;
		case PCI_DEVICE_ID_LSI_VENTURA:
		case PCI_DEVICE_ID_LSI_CRUSADER:
		case PCI_DEVICE_ID_LSI_HARPOON:
		case PCI_DEVICE_ID_LSI_TOMCAT:
		case PCI_DEVICE_ID_LSI_VENTURA_4PORT:
		case PCI_DEVICE_ID_LSI_CRUSADER_4PORT:
			instance->adapter_type = VENTURA_SERIES;
			break;
		case PCI_DEVICE_ID_LSI_FUSION:
		case PCI_DEVICE_ID_LSI_PLASMA:
			instance->adapter_type = THUNDERBOLT_SERIES;
			break;
		case PCI_DEVICE_ID_LSI_INVADER:
		case PCI_DEVICE_ID_LSI_INTRUDER:
		case PCI_DEVICE_ID_LSI_INTRUDER_24:
		case PCI_DEVICE_ID_LSI_CUTLASS_52:
		case PCI_DEVICE_ID_LSI_CUTLASS_53:
		case PCI_DEVICE_ID_LSI_FURY:
			instance->adapter_type = INVADER_SERIES;
			break;
		default: /* For all other supported controllers */
			instance->adapter_type = MFI_SERIES;
			break;
		}
	}
}