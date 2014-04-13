int setLogLevel(int logLevel) {

	switch (logLevel) {
	case 7:
		undefLogLevel();
#define LOGLEVEL_DEBUG 7
#define LOGLEVEL_INFORMATIONAL 6
#define LOGLEVEL_NOTICE 5
#define LOGLEVEL_WARNING 4
#define LOGLEVEL_ERROR 3
#define LOGLEVEL_CRITICAL 2
#define LOGLEVEL_ALERT 1
#define LOGLEVEL_EMERGENCY 0
		printf("Set Log-Level to %i ...", LOGLEVEL_DEBUG);
		return 1;

	case 6:
		undefLogLevel();
#define LOGLEVEL_INFORMATIONAL 6
#define LOGLEVEL_NOTICE 5
#define LOGLEVEL_WARNING 4
#define LOGLEVEL_ERROR 3
#define LOGLEVEL_CRITICAL 2
#define LOGLEVEL_ALERT 1
#define LOGLEVEL_EMERGENCY 0
		printf("Set Log-Level to %i ...", LOGLEVEL_INFORMATIONAL);
		return 1;

	case 5:
		undefLogLevel();
#define LOGLEVEL_NOTICE 5
#define LOGLEVEL_WARNING 4
#define LOGLEVEL_ERROR 3
#define LOGLEVEL_CRITICAL 2
#define LOGLEVEL_ALERT 1
#define LOGLEVEL_EMERGENCY 0
		printf("Set Log-Level to %i ...", LOGLEVEL_NOTICE);
		return 1;

	case 4:
		undefLogLevel();
#define LOGLEVEL_WARNING 4
#define LOGLEVEL_ERROR 3
#define LOGLEVEL_CRITICAL 2
#define LOGLEVEL_ALERT 1
#define LOGLEVEL_EMERGENCY 0
		printf("Set Log-Level to %i ...", LOGLEVEL_WARNING);
		return 1;

	case 3:
		undefLogLevel();
#define LOGLEVEL_ERROR 3
#define LOGLEVEL_CRITICAL 2
#define LOGLEVEL_ALERT 1
#define LOGLEVEL_EMERGENCY 0
		printf("Set Log-Level to %i ...", LOGLEVEL_ERROR);
		return 1;

	case 2:
		undefLogLevel();
#define LOGLEVEL_CRITICAL 2
#define LOGLEVEL_ALERT 1
#define LOGLEVEL_EMERGENCY 0
		printf("Set Log-Level to %i ...", LOGLEVEL_CRITICAL);
		return 1;

	case 1:
		undefLogLevel();
#define LOGLEVEL_ALERT 1
#define LOGLEVEL_EMERGENCY 0
		printf("Set Log-Level to %i ...", LOGLEVEL_ALERT);
		return 1;

	case 0:
		undefLogLevel();
#define LOGLEVEL_EMERGENCY 0
		printf("Set Log-Level to %i ...", LOGLEVEL_EMERGENCY);
		return 1;

	default:
		printf("Argument was not valid. Valid inputs for Log-Level are 0 - 7. ");
		printf("Log-Level will now be set to default Log-Level.\n");
		setdefault_LogLevel();
		break;
	}
}

void setdefault_LogLevel() {
	printf("Setting up now default Log-Level to 4 = WARNING.\n");
	setLogLevel(4);
}

void undefLogLevel() {
	printf("Deleting old Log-Level before setting new Log-Level.\n");
#ifdef LOGLEVEL_DEBUG
#undef LOGLEVEL_DEBUG
#endif

#ifdef LOGLEVEL_INFORMATIONAL
#undef LOGLEVEL_INFORMATIONAL
#endif

#ifdef LOGLEVEL_NOTICE
#undef LOGLEVEL_NOTICE
#endif

#ifdef LOGLEVEL_WARNING
#undef LOGLEVEL_WARNING
#endif

#ifdef LOGLEVEL_ERROR
#undef LOGLEVEL_ERROR
#endif

#ifdef LOGLEVEL_CRITICAL
#undef LOGLEVEL_CRITICAL
#endif

#ifdef	LOGLEVEL_ALERT
#undef LOGLEVEL_ALERT
#endif

#ifdef LOGLEVEL_EMERGENCY
#undef LOGLEVEL_EMERGENCY
#endif
}

