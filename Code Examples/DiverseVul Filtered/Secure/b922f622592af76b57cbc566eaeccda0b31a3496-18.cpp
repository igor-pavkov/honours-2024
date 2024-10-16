int hw_atl_utils_mpi_get_link_status(struct aq_hw_s *self)
{
	struct aq_hw_link_status_s *link_status = &self->aq_link_status;
	u32 mpi_state;
	u32 speed;

	mpi_state = hw_atl_utils_mpi_get_state(self);
	speed = mpi_state >> HW_ATL_MPI_SPEED_SHIFT;

	if (!speed) {
		link_status->mbps = 0U;
	} else {
		switch (speed) {
		case HAL_ATLANTIC_RATE_10G:
			link_status->mbps = 10000U;
			break;

		case HAL_ATLANTIC_RATE_5G:
		case HAL_ATLANTIC_RATE_5GSR:
			link_status->mbps = 5000U;
			break;

		case HAL_ATLANTIC_RATE_2G5:
			link_status->mbps = 2500U;
			break;

		case HAL_ATLANTIC_RATE_1G:
			link_status->mbps = 1000U;
			break;

		case HAL_ATLANTIC_RATE_100M:
			link_status->mbps = 100U;
			break;

		default:
			return -EBUSY;
		}
	}
	link_status->full_duplex = true;

	return 0;
}