/*
 * Logging.h
 *
 *  Created on: 2018. máj. 26.
 *      Author: tomikaa
 */

#ifndef HUB_LOGGING_H_
#define HUB_LOGGING_H_

#include <stdio.h>

#undef ENABLE_DEBUG_LOG
#define ENABLE_INFO_LOG

#ifdef ENABLE_DEBUG_LOG
#define DEBUG(category, ...) { printf("<D>[%s] ", #category); printf(__VA_ARGS__); printf("\r\n"); }
#else
#define DEBUG(category, ...)
#endif

#if defined(ENABLE_INFO_LOG) || defined(ENABLE_DEBUG_LOG)
#define INFO(category, ...) { printf("[%s] ", #category); printf(__VA_ARGS__); printf("\r\n"); }
#else
#define INFO(category, ...)
#endif


#endif /* HUB_LOGGING_H_ */
