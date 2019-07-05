#include "falldector.h"
#include <sensor.h>
#include <bundle.h>
#include <message_port.h>
#include <package_manager.h>
#include <app_manager.h>
#include <app_control.h>
#include <net_connection.h>
#include <feedback.h>
//#include <string>
//#include <vector>
#include <glib.h>
#include <curl/curl.h>

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
	Eina_List *_gait_list;
	int sensor_data_count;
	Evas_Object *popup;
} appdata_s;

typedef struct{
	//int cnt;
	float ax;
	float ay;
	float az;
	unsigned long long stimestamp;
} full_sensor_data;

static sensor_listener_h listener;
char buf[256] = {0};

pthread_t _gaitThreadID = -1;
Ecore_Pipe *_gaitPipe = NULL;

#define FD_MESSAGE_PORT_CALLEE_APPID	"com.samsung.falldectorservice"
#define FD_MESSAGE_PORT	"com.samsung.falldectorservice"

#define SERVER_URL "http://109.123.123.47:8080"
#define GAIT_DATA_DIR "/opt/usr/media/Downloads"
#define ESP (0.0000001)
#define TEST_FILE "/opt/usr/apps/com.samsung.falldetector/data/hi.txt"

#define FALL_DETECTION_PHRASE "attention, fall dectected"

char *g_proxy_addr = NULL;
bool g_netstat = FALSE;
connection_h g_conn_handle;

int checkServiceRunning()
{

	bool running = false;
	int i = 0;
	int ret = 0;

	for (i = 0; i < 10; i++)
	{
		ret = app_manager_is_running(FD_MESSAGE_PORT_CALLEE_APPID, &running);

		if (!running)
		{
			app_control_h control;
			ret = app_control_create(&control);

			ret = app_control_set_app_id(control, FD_MESSAGE_PORT_CALLEE_APPID);

			ret = app_control_send_launch_request(control, NULL, NULL);

			ret = app_control_destroy(control);

			usleep(200000);
		}
		else
		{
			break;
		}
	}

	if (i == 10)
	{
		return 0;
	}

	return ret;
}


void messagePortCallback(int local_port_id, const char *remote_app_id, const char *remote_port, bool trusted_remote_port, bundle *messagem, void *user_data)
{
	char *val = NULL;
	char *pError = NULL;
	int ret = 0;
	//std::string errorCode;

	ret = bundle_get_str(messagem, "RESULT", &val);

}

static void __network_proxy_change_cb(const char *ipv4_address, const char *ipv6_address, void *user_data)
{
//	FALLDECTOR_FUNC_ENTER();
	if (ipv4_address) {
		free(g_proxy_addr);
		g_proxy_addr = g_strdup(ipv4_address);
		FALLDECTOR_LOGD("g_proxy_addr:%s", g_proxy_addr);
	}
}

void initMessagePort()
{
	int ret = 0;
	ret = message_port_register_local_port(FD_MESSAGE_PORT, messagePortCallback, NULL);
}

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}


static size_t _gather_header_data(void *downloaded_data,
		size_t size,
		size_t nmemb,
		void *user_data)
{
    //WENTER();
	size_t total_size = size * nmemb;
	g_byte_array_append((GByteArray *)user_data, (const unsigned char *) downloaded_data, total_size);
	//WINFO("total_size=[%ld]", total_size);
	return total_size;
}

static size_t _gather_data(void *downloaded_data,
		 size_t size,
		 size_t nmemb,
		 void *user_data)
 {
	FALLDECTOR_FUNC_ENTER();
	size_t total_size = size * nmemb;
	g_byte_array_append((GByteArray *)user_data, (const unsigned char *) downloaded_data, total_size);
	FALLDECTOR_LOGD("total_size=[%ld]", total_size);
	return total_size;
 }

 static int _curl_debug_cb(CURL *curl, curl_infotype type, char *buffer, size_t size, void *user_data)
 {
	 return 0;
 }

void _curl_set_common_option(CURL *curl,
		const char *url,
		GByteArray **response_header_ptr,
		GByteArray **response_body_ptr)
{

	//common
	FALLDECTOR_FUNC_ENTER();

	CURLcode ret;
	ret = curl_easy_setopt(curl, CURLOPT_URL, url);
	if (CURLE_OK != ret) {
		FALLDECTOR_LOGD("curl_easy_setopt: CURLOPT_URL failed!! ret[%d]", ret);
	}
	//head
	GByteArray *response_header_data = g_byte_array_new();
	ret = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, _gather_header_data);
	if (CURLE_OK != ret) {
		FALLDECTOR_LOGD(
				"curl_easy_setopt: CURLOPT_HEADERFUNCTION failed!! ret[%d]",
				ret);
	}

	ret = curl_easy_setopt(curl, CURLOPT_HEADERDATA, response_header_data);
	if (CURLE_OK != ret) {
		FALLDECTOR_LOGD("curl_easy_setopt: CURLOPT_HEADERDATA failed!! ret[%d]",
				ret);
	}

	//body
	GByteArray *response_body_data = g_byte_array_new();

	ret = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _gather_data);
	if (CURLE_OK != ret) {
		FALLDECTOR_LOGD(
				"curl_easy_setopt: CURLOPT_WRITEFUNCTION failed!! ret[%d]",
				ret);
	}

	ret = curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_body_data);
	if (CURLE_OK != ret) {
		FALLDECTOR_LOGD("curl_easy_setopt: CURLOPT_WRITEDATA failed!! ret[%d]",
				ret);
	}

	*response_body_ptr = response_body_data;

	ret = curl_easy_setopt(curl, CURLOPT_TIMEOUT, 500);
	if (CURLE_OK != ret) {
		FALLDECTOR_LOGD("curl_easy_setopt: CURLOPT_TIMEOUT_MS failed!! ret[%d]",
				ret);
	}

	ret = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	if (CURLE_OK != ret) {
		FALLDECTOR_LOGD("curl_easy_setopt: CURLOPT_VERBOSE failed!! ret[%d]",
				ret);
	}

	ret = curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, _curl_debug_cb);
	if (CURLE_OK != ret) {
		FALLDECTOR_LOGD(
				"curl_easy_setopt: CURLOPT_DEBUGFUNCTION failed!! ret[%d]",
				ret);
	}

	ret = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
	if (CURLE_OK != ret) {
		FALLDECTOR_LOGD(
				"curl_easy_setopt: CURLOPT_SSL_VERIFYPEER failed!! ret[%d]",
				ret);
	}
//char *g_proxy_addr = NULL;
	bool g_netstat = FALSE;
	connection_h g_conn_handle;
//	ret = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
//	if (CURLE_OK != ret) {
//		FALLDECTOR_LOGD(
//				"curl_easy_setopt: CURLOPT_SSL_VERIFYHOST failed!! ret[%d]",
//				ret);
//	}
//
//	ret = curl_easy_setopt(curl, CURLOPT_CERTINFO, 0L);
//	if (CURLE_OK != ret) {
//		FALLDECTOR_LOGD("curl_easy_setopt: CURLOPT_CERTINFO failed!! ret[%d]",
//				ret);
//	}

	FALLDECTOR_FUNC_LEAVE();
}

void *_thread_cb(void *data)
{
	FALLDECTOR_FUNC_ENTER();
	FALLDECTOR_LOGD("g_proxy_addr is [%s]", g_proxy_addr);
	CURL *curl = curl_easy_init();
	CURLcode ret;
//	CURLcode ret = curl_easy_setopt(curl, CURLOPT_PROXY, g_proxy_addr);
//	if (CURLE_OK != ret) {
//		FALLDECTOR_LOGD("curl_easy_setopt: CURLOPT_PROXY failed!! ret[%d]", ret);
//	}

	struct stat file_stat;
	int fd = -1;
	const char *file_path = (const char *)TEST_FILE;
	if (0 > (fd = open(file_path, O_RDONLY))) {
		FALLDECTOR_LOGD("Open file failed");
	}
	if (0 != fstat(fd, &file_stat)) {
		FALLDECTOR_LOGD("File not exist");
		close(fd);
	}
	unsigned long long total_size = (unsigned long long)file_stat.st_size;
	FALLDECTOR_LOGD("total_size:%llu", total_size);
	char *body = (char*)malloc(sizeof(char)*total_size+1);
	//size_t rsize=(size_t)read(fd, (void *)body, total_size);
	(void)read(fd, (void *)body, total_size);
	close(fd);

	struct curl_slist *headers = NULL;
	char *base_name = g_path_get_basename(file_path);
	FALLDECTOR_LOGD("base_name:%s", base_name);
	char *tmp_header = g_strconcat("Content-Type", ":", "application/octet-stream", ":", base_name, "endname",NULL);
	headers = curl_slist_append(headers, tmp_header);
	free(tmp_header);
	ret = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	if (CURLE_OK != ret) {
		FALLDECTOR_LOGD("curl_easy_setopt: CURLOPT_HTTPHEADER failed!! ret[%d]", ret);
	}

	GByteArray *response_header = NULL;
	GByteArray *response_body = NULL;
	_curl_set_common_option(curl, SERVER_URL, &response_header, &response_body);
	ret = curl_easy_setopt(curl, CURLOPT_POST, 1L);
	if (CURLE_OK != ret) {
		FALLDECTOR_LOGD("curl_easy_setopt: CURLOPT_POST failed!! ret[%d]", ret);
	}
	ret = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, total_size);
	if (CURLE_OK != ret) {
		FALLDECTOR_LOGD("curl_easy_setopt: CURLOPT_POSTFIELDSIZE failed!! ret[%d]", ret);
	}
	ret = curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, body);
	if (CURLE_OK != ret) {
		FALLDECTOR_LOGD("curl_easy_setopt: CURLOPT_POSTFIELDS failed!! ret[%d]", ret);
	}

	FALLDECTOR_LOGD("easy perform before");
	CURLcode curl_code = curl_easy_perform(curl);
	FALLDECTOR_LOGD("Curl Perform Receive: [%d][%s]", curl_code, curl_easy_strerror(curl_code));
	free(body);
	char *head_data = NULL;
	if (response_header) {
		head_data = g_strndup((const char *)response_header->data, response_header->len);
		FALLDECTOR_LOGD("head: %s",head_data);
	}
	char *body_data = NULL;
	if (response_body) {
		 body_data = g_strndup((const char *)response_body->data, response_body->len);
		 FALLDECTOR_LOGD("body: %s",body_data);
		 if(body_data) {
			 ecore_pipe_write(_gaitPipe, body_data, strlen(body_data)+1);
		 }
		 //check result
	}

	curl_easy_cleanup(curl);
	//end the pipe

	FALLDECTOR_FUNC_LEAVE();
	return NULL;
}


static void _timeoutCb(void *data, Evas_Object *obj, void *event_info)
{
	int ret;

	appdata_s *app_data = (appdata_s *)data;

	if (!app_data) return;

	app_data->popup = NULL;

	ret = feedback_deinitialize();
	if (ret != 0) {
		FALLDECTOR_LOGD("feedback_initialize failed!");
		return;
	}

}

void
_feed_back()
{
	FALLDECTOR_FUNC_ENTER();

	int ret;

	ret = feedback_initialize();
	if (ret != 0) {
		FALLDECTOR_LOGD("feedback_initialize failed!");
		return;
	}

	feedback_play_type(FEEDBACK_TYPE_VIBRATION, FEEDBACK_PATTERN_GENERAL_STRONG_BUZZ);

//	ret = feedback_deinitialize();
//	if (ret != 0) {
//		FALLDECTOR_LOGD("feedback_initialize failed!");
//		return;
//	}

	FALLDECTOR_FUNC_LEAVE();
}

void
_show_popup(void *data, char *popupText)
{
	FALLDECTOR_FUNC_ENTER();
	appdata_s *app_data = (appdata_s *)data;

	if (!app_data) return;

	if((app_data->popup)) {
		FALLDECTOR_LOGD("popup already shows");
		return;
	}

	Evas_Object *popup = elm_popup_add(app_data->win);

	app_data->popup = popup;
	elm_object_style_set(popup, "toast/circle");
//        eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, _popup_hide_cb, NULL);
//        evas_object_smart_callback_add(popup, "dismissed", _popup_hide_finished_cb, NULL);

	elm_object_part_text_set(popup, "elm.text", popupText);
	elm_popup_timeout_set(popup, 2.0);
	evas_object_smart_callback_add(popup, "timeout", _timeoutCb, data);

	evas_object_show(popup);

	_feed_back();

	FALLDECTOR_FUNC_LEAVE();
}


void _pipe_cb(void *data, void *buffer, unsigned int nbyte)
{
	FALLDECTOR_FUNC_ENTER();

	char *extraData = (char *)buffer;

	FALLDECTOR_LOGD("===extraData=%s=", extraData);

	if(extraData && strncmp(extraData, FALL_DETECTION_PHRASE, strlen(FALL_DETECTION_PHRASE)) == 0)
	{
//		appdata_s *ad = (appdata_s *)user_data;
		_show_popup(data, FALL_DETECTION_PHRASE);
	}

	FALLDECTOR_FUNC_LEAVE();

}



Eina_Bool send_file(void* data)
{

	appdata_s *ad = (appdata_s *)data;

	if(!ad) {
			FALLDECTOR_LOGD("=================ad is null============");
		}


	FALLDECTOR_FUNC_ENTER();
	_gaitPipe=ecore_pipe_add(_pipe_cb, data);
	pthread_mutex_t thread_mutex;
	pthread_mutex_init(&thread_mutex, NULL);
	pthread_mutex_lock(&thread_mutex);
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);
	if (pthread_create(&_gaitThreadID, NULL, _thread_cb, NULL) != 0) {
		FALLDECTOR_LOGD("pthread_create failed");
	}
	pthread_attr_destroy(&tattr);
	pthread_mutex_unlock(&thread_mutex);
	pthread_mutex_destroy(&thread_mutex);
	return FALSE;
}

Eina_Bool write_file(void* data)
{
	appdata_s *ad = (appdata_s *)data;

	if(!ad) {
		FALLDECTOR_LOGD("=================ad is null============");
	}

	Eina_List *l = NULL;
	full_sensor_data *nd = NULL;
	int ccnt = 0;

//	remove(TEST_FILE);//remove old file

	FILE *fp = fopen(TEST_FILE,"w");//fname
	FALLDECTOR_RETVM_IF(!fp, false, "write file failed");
//	int buffer[] = {1, 2, 3, 4};
//	fwrite (buffer , sizeof(int), 4, fp);
#if 1
	void *it;
	EINA_LIST_FOREACH(ad->_gait_list, l, it) {
		nd = (full_sensor_data *)it;
		if (!nd) continue;;
		fprintf(fp, "%f,%f,%f,%llu\n", nd->ax, nd->ay, nd->az, nd->stimestamp);

		ccnt++;
	}
#endif
	fclose(fp);

	ecore_timer_add(0.3, send_file, data);//must wait 3s,due to incomplete file writing

}


void
example_sensor_callback(sensor_h sensor, sensor_event_s *event, void *user_data)
{

    /*
       If a callback is used to listen for different sensor types,
       it can check the sensor type
    */
    sensor_type_e type;
    sensor_get_type(sensor, &type);

    appdata_s *ad = (appdata_s *)user_data;

    if (type == SENSOR_ACCELEROMETER) {
    	ad->sensor_data_count++;
        unsigned long long timestamp = event->timestamp;
        int accuracy = event->accuracy;
        float x = event->values[0];
        float y = event->values[1];
        float z = event->values[2];

        snprintf(buf, sizeof(buf),"<align=center>Fall dector <br><br>accelerometer <br>x :%.5f, <br>y: %.5f, <br>z: %.5f</align>", x, y, z);
        elm_object_text_set(ad->label, buf);

        //write to file every 10 second
        full_sensor_data *node = (full_sensor_data*)calloc(1, sizeof(full_sensor_data));
        node->ax = x;
        node->ay = y;
        node->az = z;
        node->stimestamp = timestamp;


//        FALLDECTOR_LOGD("=======sensor data:[x=%.5f, y=%.5f, z=%.5f]============",x,y,z);

        ad->_gait_list = eina_list_append(ad->_gait_list, node);

        if(ad->sensor_data_count > 1200) {
        	//save data to file and send to server
        	ad->sensor_data_count = 0;

        	FALLDECTOR_LOGD("=====sample==sensor data:[x=%.5f, y=%.5f, z=%.5f]============",x,y,z);

        	write_file(user_data);

        	eina_list_free(ad->_gait_list);
        }
    }
}

static void
create_base_gui(appdata_s *ad)
{
	/* Window */
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	/* Conformant */
	/* Create and initialize elm_conformant.
	   elm_conformant is mandatory for base gui to have proper size
	   when indicator or virtual keypad is visible. */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/* Label */
	/* Create an actual view of the base gui.
	   Modify this part to change the view. */
	ad->label = elm_label_add(ad->conform);
	elm_object_text_set(ad->label, "<align=center>Fall dector</align>");
	evas_object_size_hint_weight_set(ad->label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_content_set(ad->conform, ad->label);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);



	bool supported = false;

	sensor_is_supported(SENSOR_ACCELEROMETER, &supported);
	if (!supported) {
	    /* Accelerometer is not supported on the current device */
		return;
	}

	sensor_h sensor;
	sensor_get_default_sensor(SENSOR_ACCELEROMETER, &sensor);

	sensor_create_listener(sensor, &listener);


	sensor_listener_set_event_cb(listener, 10, example_sensor_callback, (void *)ad);


	sensor_listener_start(listener);

	sensor_listener_set_interval(listener, 10);

	sensor_listener_set_attribute_int(listener, SENSOR_ATTRIBUTE_PAUSE_POLICY, SENSOR_PAUSE_NONE);


	sensor_recorder_is_supported(SENSOR_HUMAN_PEDOMETER, &supported);
	if (!supported) {
	    /* Pedometer is not supported or cannot be recorded on the current device */
		return;
	}


	sensor_recorder_option_h option;

	sensor_recorder_create_option(&option);
	/* 720 hours (30 days) */
	sensor_recorder_option_set_int(option, SENSOR_RECORDER_OPTION_RETENTION_PERIOD, 30 * 24);

}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = data;

	create_base_gui(ad);

	return true;
}

static void __network_state_change_cb(connection_type_e type, void *user_data)
{
	if (CONNECTION_TYPE_DISCONNECTED == type) {
		FALLDECTOR_LOGD("Disconnected from network ");
		g_netstat = FALSE;
	} else {
		FALLDECTOR_LOGD("Reconnected to network ");
		g_netstat = TRUE;
	}
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */

	if (!g_conn_handle) {//init in first time
		int retcode = connection_create(&(g_conn_handle));
//		GEARSTORE_RETM_IF(CONNECTION_ERROR_NONE != retcode, "CONNECTION_ERROR_NONE != ret");

		char *proxy_address = NULL;
		connection_get_proxy(g_conn_handle, CONNECTION_ADDRESS_FAMILY_IPV4, &proxy_address);
		FALLDECTOR_LOGD("================proxy_address========%s=====",proxy_address);
		if (proxy_address) {
			__network_proxy_change_cb(proxy_address, NULL, NULL);
			free(proxy_address);
		}

		connection_type_e type = CONNECTION_TYPE_DISCONNECTED;
		connection_get_type(g_conn_handle, &type);
		FALLDECTOR_LOGD("type[%d]", type);
		if (type != CONNECTION_TYPE_DISCONNECTED) {
			g_netstat = TRUE;
		}
		connection_set_type_changed_cb(g_conn_handle, __network_state_change_cb, NULL);
		connection_set_proxy_address_changed_cb(g_conn_handle, __network_proxy_change_cb, NULL);

	}
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
	sensor_listener_stop(listener);
	sensor_destroy_listener(listener);
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
