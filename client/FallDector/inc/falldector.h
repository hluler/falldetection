#ifndef __falldector_H__
#define __falldector_H__

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>
#include <sys/syscall.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

//#include <dlog/dlog-internal.h>


#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "FALLDECECTOR"

#if !defined(PACKAGE)
#define PACKAGE "com.samsung.falldetector"
#endif

#endif /* __falldector_H__ */


/* anci c color type */
#define FONT_COLOR_RESET    "\033[0m"
#define FONT_COLOR_RED      "\033[31m"
#define FONT_COLOR_GREEN    "\033[32m"
#define FONT_COLOR_YELLOW   "\033[33m"
#define FONT_COLOR_BLUE     "\033[34m"
#define FONT_COLOR_PURPLE   "\033[35m"
#define FONT_COLOR_CYAN     "\033[36m"
#define FONT_COLOR_GRAY     "\033[37m"

#define FONT_COLOR_BLUE_CAP "\033[104m"
#define FONT_COLOR_GRAY_CAP "\033[100m"

static pid_t gettid(void)
{
	return syscall(__NR_gettid);
}


//extern char *g_proxy_addr;

#define FALLDECTOR_LOGD(fmt, args...)		LOGI(FONT_COLOR_RESET"[T:%d] " fmt "" FONT_COLOR_RESET, gettid(), ##args)
#define FALLDECTOR_LOGW(fmt, args...)		LOGI(FONT_COLOR_YELLOW"[T:%d]" fmt "" FONT_COLOR_RESET, gettid(), ##args)
#define FALLDECTOR_LOGE(fmt, args...)		LOGI(FONT_COLOR_RED"[T:%d] " fmt "" FONT_COLOR_RESET, gettid(), ##args)
#define FALLDECTOR_FUNC_ENTER()		LOGI(FONT_COLOR_GREEN"[T:%d] <<< enter!!" FONT_COLOR_RESET, gettid())
#define FALLDECTOR_FUNC_LEAVE()		LOGI(FONT_COLOR_GREEN"[T:%d] >>> leave!!" FONT_COLOR_RESET, gettid())

#define FALLDECTOR_LOGE_IF(expr, fmt, args...) do { \
		if (expr) { \
			FALLDECTOR_LOGE("[%s] Error, message " fmt, #expr, ##args);\
		} \
	} while (0)


#define FALLDECTOR_RET_IF(expr) do { \
		if (expr) { \
			FALLDECTOR_LOGE("[%s] Return", #expr);\
			return; \
		} \
	} while (0)

#define FALLDECTOR_RETV_IF(expr, val) do { \
		if (expr) { \
			FALLDECTOR_LOGE("[%s] Return value", #expr);\
			return (val); \
		} \
	} while (0)
#define FALLDECTOR_RETM_IF(expr, fmt, args...) do { \
		if (expr) { \
			FALLDECTOR_LOGE("[%s] Return, message " fmt, #expr, ##args);\
			return; \
		} \
	} while (0)
#define FALLDECTOR_RETVM_IF(expr, val, fmt, args...) do { \
		if (expr) { \
			FALLDECTOR_LOGE("[%s] Return value, message " fmt, #expr, ##args);\
			return (val); \
		} \
	} while (0)

#define FALLDECTOR_TRYV_IF(expr, val) do { \
		if (expr) { \
			FALLDECTOR_LOGE("[%s] Return value", #expr);\
			val;	\
			goto CATCH; 	\
		} \
	} while (0)
#define FALLDECTOR_TRYM_IF(expr, fmt, arg...) do {   \
		if (expr) { \
			FALLDECTOR_LOGE("[%s] goto CATCH " fmt, #expr, ##arg);	\
			goto CATCH; 	\
		}		\
	} while(0)
#define FALLDECTOR_TRYVM_IF(expr, val, fmt, arg...) do {   \
		if (expr) { \
			FALLDECTOR_LOGE("[%s] goto CATCH " fmt, #expr, ##arg);	\
			val;	\
			goto CATCH; 	\
		}		\
	} while(0)
