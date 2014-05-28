enum LOGLEVELS {
	LOG_EMERGENCY,
	LOG_ALERT,
	LOG_CRITICAL,
	LOG_ERROR,
	LOG_WARNING,
	LOG_NOTICE,
	LOG_INFORMATIONAL,
	LOG_DEBUG
} LOG_LEVEL;

/* returns the log-level compared to the given int */
char * getLogLevel(int log_level, char *lvl) {
	switch (log_level) {
	case LOG_EMERGENCY:
		lvl = "LOG_EMERGENCY    ";
		break;
	case LOG_ALERT:
		lvl = "LOG_ALERT        ";
		break;
	case LOG_CRITICAL:
		lvl = "LOG_CRITICAL     ";
		break;
	case LOG_ERROR:
		lvl = "LOG_ERROR        ";
		break;
	case LOG_WARNING:
		lvl = "LOG_WARNING      ";
		break;
	case LOG_NOTICE:
		lvl = "LOG_NOTICE       ";
		break;
	case LOG_INFORMATIONAL:
		lvl = "LOG_INFORMATIONAL";
		break;
	case LOG_DEBUG:
		lvl = "LOG_DEBUG        ";
		break;
	}
	return lvl;
}

