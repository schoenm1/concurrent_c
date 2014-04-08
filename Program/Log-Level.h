#define LOGLEVEL_ARG "-l";

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
		printf("set Log-Level to %i\n", LOGLEVEL_DEBUG);
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
		printf("set Log-Level to %i\n", LOGLEVEL_INFORMATIONAL);
		return 1;

	case 5:
		undefLogLevel();
#define LOGLEVEL_NOTICE 5
#define LOGLEVEL_WARNING 4
#define LOGLEVEL_ERROR 3
#define LOGLEVEL_CRITICAL 2
#define LOGLEVEL_ALERT 1
#define LOGLEVEL_EMERGENCY 0
		printf("set Log-Level to %i\n", LOGLEVEL_NOTICE);
		return 1;

	case 4:
		undefLogLevel();
#define LOGLEVEL_WARNING 4
#define LOGLEVEL_ERROR 3
#define LOGLEVEL_CRITICAL 2
#define LOGLEVEL_ALERT 1
#define LOGLEVEL_EMERGENCY 0
		printf("set Log-Level to %i\n", LOGLEVEL_WARNING);
		return 1;

	case 3:
		undefLogLevel();
#define LOGLEVEL_ERROR 3
#define LOGLEVEL_CRITICAL 2
#define LOGLEVEL_ALERT 1
#define LOGLEVEL_EMERGENCY 0
		printf("set Log-Level to %i\n", LOGLEVEL_ERROR);
		return 1;

	case 2:
		undefLogLevel();
#define LOGLEVEL_CRITICAL 2
#define LOGLEVEL_ALERT 1
#define LOGLEVEL_EMERGENCY 0
		printf("set Log-Level to %i\n", LOGLEVEL_CRITICAL);
		return 1;

	case 1:
		undefLogLevel();
#define LOGLEVEL_ALERT 1
#define LOGLEVEL_EMERGENCY 0
		printf("set Log-Level to %i\n", LOGLEVEL_ALERT);
		return 1;

	case 0:
		undefLogLevel();
#define LOGLEVEL_EMERGENCY 0
		printf("set Log-Level to %i\n", LOGLEVEL_EMERGENCY);
		return 1;

	default:
		printf("Argument was not valid. Valid inputs for LOGLEVELS are 0 - 7\n");
		break;
	}

}

void undefLogLevel() {
	printf("Before setting new loglevel, clear old Log-Level.\n");
#ifdef LOGLEVEL_DEBUG
#undef LOGLEVEL_DEBUG 7
#endif

#ifdef LOGLEVEL_INFORMATIONAL
#undef LOGLEVEL_INFORMATIONAL 6
#endif

#ifdef LOGLEVEL_NOTICE
#undef LOGLEVEL_NOTICE 5
#endif

#ifdef LOGLEVEL_WARNING
#undef LOGLEVEL_WARNING 4
#endif

#ifdef LOGLEVEL_ERROR
#undef LOGLEVEL_ERROR 3
#endif

#ifdef LOGLEVEL_CRITICAL
#undef LOGLEVEL_CRITICAL 2
#endif

#ifdef	LOGLEVEL_ALERT
#undef LOGLEVEL_ALERT 1
#endif

#ifdef LOGLEVEL_EMERGENCY
#undef LOGLEVEL_EMERGENCY 0
#endif
}

