static int CreateHTTPRangeResponseHeader(
	/*! String containing the range. */
	char *ByteRangeSpecifier,
	/*! Length of the file. */
	off_t FileLength,
	/*! [out] SendInstruction object where the range operations will be stored. */
	struct SendInstruction *Instr)
{
	off_t FirstByte, LastByte;
	char *RangeInput;
	char *Ptr;
	int rc = 0;

	Instr->IsRangeActive = 1;
	Instr->ReadSendSize = FileLength;
	if (!ByteRangeSpecifier)
		return HTTP_BAD_REQUEST;
	RangeInput = strdup(ByteRangeSpecifier);
	if (!RangeInput)
		return HTTP_INTERNAL_SERVER_ERROR;
	/* CONTENT-RANGE: bytes 222-3333/4000  HTTP_PARTIAL_CONTENT */
	if (StrStr(RangeInput, "bytes") == NULL ||
	    (Ptr = StrStr(RangeInput, "=")) == NULL) {
		free(RangeInput);
		Instr->IsRangeActive = 0;
		return HTTP_BAD_REQUEST;
	}
	/* Jump = */
	Ptr = Ptr + 1;
	if (FileLength < 0) {
		free(RangeInput);
		return HTTP_REQUEST_RANGE_NOT_SATISFIABLE;
	}
	if (GetNextRange(&Ptr, &FirstByte, &LastByte) != -1) {
		if (FileLength < FirstByte) {
			free(RangeInput);
			return HTTP_REQUEST_RANGE_NOT_SATISFIABLE;
		}
		if (FirstByte >= 0 && LastByte >= 0 && LastByte >= FirstByte) {
			if (LastByte >= FileLength)
				LastByte = FileLength - 1;
			Instr->RangeOffset = FirstByte;
			Instr->ReadSendSize = LastByte - FirstByte + 1;
			/* Data between two range. */
			rc = snprintf(Instr->RangeHeader,
				sizeof(Instr->RangeHeader),
				"CONTENT-RANGE: bytes %" PRId64
				"-%" PRId64 "/%" PRId64 "\r\n",
				(int64_t)FirstByte,
				(int64_t)LastByte,
				(int64_t)FileLength);
			if (rc < 0 || (unsigned int) rc >= sizeof(Instr->RangeHeader)) {
				free(RangeInput);
				return HTTP_INTERNAL_SERVER_ERROR;
			}
		} else if (FirstByte >= 0 && LastByte == -1
			   && FirstByte < FileLength) {
			Instr->RangeOffset = FirstByte;
			Instr->ReadSendSize = FileLength - FirstByte;
			rc = snprintf(Instr->RangeHeader,
				sizeof(Instr->RangeHeader),
				"CONTENT-RANGE: bytes %" PRId64
				"-%" PRId64 "/%" PRId64 "\r\n",
				(int64_t)FirstByte,
				(int64_t)(FileLength - 1),
				(int64_t)FileLength);
			if (rc < 0 || (unsigned int) rc >= sizeof(Instr->RangeHeader)) {
				free(RangeInput);
				return HTTP_INTERNAL_SERVER_ERROR;
			}
		} else if (FirstByte == -1 && LastByte > 0) {
			if (LastByte >= FileLength) {
				Instr->RangeOffset = 0;
				Instr->ReadSendSize = FileLength;
				rc = snprintf(Instr->RangeHeader,
					sizeof(Instr->RangeHeader),
					"CONTENT-RANGE: bytes 0-%" PRId64
					"/%" PRId64 "\r\n",
					(int64_t)(FileLength - 1),
					(int64_t)FileLength);
			} else {
				Instr->RangeOffset = FileLength - LastByte;
				Instr->ReadSendSize = LastByte;
				rc = snprintf(Instr->RangeHeader,
					sizeof(Instr->RangeHeader),
					"CONTENT-RANGE: bytes %" PRId64
					"-%" PRId64 "/%" PRId64 "\r\n",
					(int64_t)(FileLength - LastByte),
					(int64_t)FileLength - 1,
					(int64_t)FileLength);
			}
			if (rc < 0 || (unsigned int) rc >= sizeof(Instr->RangeHeader)) {
				free(RangeInput);
				return HTTP_INTERNAL_SERVER_ERROR;
			}
		} else {
			free(RangeInput);
			return HTTP_REQUEST_RANGE_NOT_SATISFIABLE;
		}
	} else {
		free(RangeInput);
		return HTTP_REQUEST_RANGE_NOT_SATISFIABLE;
	}

	free(RangeInput);
	return HTTP_OK;
}