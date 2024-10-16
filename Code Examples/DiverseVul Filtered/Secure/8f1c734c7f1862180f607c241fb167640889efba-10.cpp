        Status makeError(std::string baseMsg, BSONElement idElem) {
            if (idElem.eoo()) {
                baseMsg += " in object with unknown _id";
            }
            else {
                baseMsg += " in object with " + idElem.toString(/*field name=*/true, /*full=*/true);
            }
            return Status(ErrorCodes::InvalidBSON, baseMsg);
        }