/*=============================================================================
 Copyright (C) 2009 Ryan Hope <rmh3093@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 =============================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "precorderD.h"
#include "gstreamer.h"
#include "TPS6105X.h"
#include "luna.h"

pthread_mutex_t recording_mutex = PTHREAD_MUTEX_INITIALIZER;

void *record_video_wrapper(void *ptr) {

	RECORD_REQUEST_t *req = (RECORD_REQUEST_t *)ptr;

	LSError lserror;
	LSErrorInit(&lserror);

	if (pthread_mutex_trylock(&recording_mutex)==0) {

		int ret = record_video(req->opts);

		pthread_mutex_unlock(&recording_mutex);

		if (ret==0)
			LSMessageReply(pub_bus, req->message, "{\"returnValue\":true}", &lserror);
		else
			LSMessageReply(pub_bus, req->message, "{\"returnValue\":false}", &lserror);
		LSMessageUnref(req->message);

	} else
		LSMessageReply(pub_bus, req->message, "{\"returnValue\":false,\"errorText\":\"Could not aquire mutex lock. Recording in progress.\"}", &lserror);

	if (LSErrorIsSet(&lserror)) {
		LSErrorPrint(&lserror, stderr);
		LSErrorFree(&lserror);
	}

	free(req->opts);
	free(req);

}

bool set_led(LSHandle* lshandle, LSMessage *message, void *ctx) {

	LSError lserror;
	LSErrorInit(&lserror);

	json_t *root = json_parse_document(LSMessageGetPayload(message));

	json_t *value_label = 0;
	value_label = json_find_first_label(root, "value");

	if (value_label==0)
		goto error_param;

	int value = atoi(value_label->child->text);

	json_free_value(&root);

	int avin = -1, mode = -1;
	TPS6105X_TORCH_CURRENT_t new_value = -1;

	if (value>0) {
		avin = TPS6105X_avin(1);
		switch (value) {
		case TPS6105X_TORCH_CURRENT_1:
			new_value = TPS6105X_torch_current(TPS6105X_TORCH_CURRENT_1);
			break;
		case TPS6105X_TORCH_CURRENT_2:
			new_value = TPS6105X_torch_current(TPS6105X_TORCH_CURRENT_2);
			break;
		case TPS6105X_TORCH_CURRENT_3:
			new_value = TPS6105X_torch_current(TPS6105X_TORCH_CURRENT_3);
			break;
		case TPS6105X_TORCH_CURRENT_4:
			new_value = TPS6105X_torch_current(TPS6105X_TORCH_CURRENT_4);
			break;
		case TPS6105X_TORCH_CURRENT_5:
			new_value = TPS6105X_torch_current(TPS6105X_TORCH_CURRENT_5);
			break;
		case TPS6105X_TORCH_CURRENT_6:
			new_value = TPS6105X_torch_current(TPS6105X_TORCH_CURRENT_6);
			break;
		case TPS6105X_TORCH_CURRENT_7:
			new_value = TPS6105X_torch_current(TPS6105X_TORCH_CURRENT_7);
			break;
		default:
			goto error_current;
		}
		mode = TPS6105X_mode(TPS6105X_MODE_1);
	} else {
		avin = TPS6105X_avin(-1);
		if (avin=='1') {
			mode = TPS6105X_mode(TPS6105X_MODE_0);
			new_value = TPS6105X_torch_current(TPS6105X_TORCH_CURRENT_0);
			avin = TPS6105X_avin(0);
		}
	}

	char *jsonResponse = 0;
	int len = 0;
	len = asprintf(&jsonResponse, "{\"returnValue\":true,\"avin\":%c,\"mode\":%d,\"value\":%d}", avin, mode, new_value);
	if (jsonResponse) {
		LSMessageReply(lshandle, message, jsonResponse, &lserror);
		free(jsonResponse);
	} else {
		LSMessageReply(lshandle, message, "{\"returnValue\":false,\"errorText\":\"asprintf failure\"}", &lserror);
	}
	goto end;

	error_param:
	LSMessageReply(lshandle, message, "{\"returnValue\":false,\"errorText\":\"Missing or invalid required param(s): value\"}", &lserror);
	goto end;

	error_current:
	LSMessageReply(lshandle, message, "{\"returnValue\":false,\"errorText\":\"Invalid torch current\"}", &lserror);

	end:
	if (LSErrorIsSet(&lserror)) {
		LSErrorPrint(&lserror, stderr);
		LSErrorFree(&lserror);
	}

	return TRUE;

}

bool start_record(LSHandle* lshandle, LSMessage *message, void *ctx) {

	LSMessageRef(message);

	RECORD_REQUEST_t *req = malloc(sizeof(RECORD_REQUEST_t));
	req->opts = malloc(sizeof(PIPELINE_OPTS_t));
	req->message = message;

	json_t *root = json_parse_document(LSMessageGetPayload(message));

	json_t *data_throughput			= json_find_first_label(root, "data_throughput");
	json_t *num_buffers				= json_find_first_label(root, "num_buffers");
	json_t *video_format			= json_find_first_label(root, "video_format");
	json_t *video_bitrate			= json_find_first_label(root, "video_bitrate");
	json_t *audio_sampling_rate		= json_find_first_label(root, "audio_sampling_rate");
	json_t *audio_encoding			= json_find_first_label(root, "audio_encoding");
	json_t *aac_stream_bitrate		= json_find_first_label(root, "aac_stream_bitrate");
	json_t *aac_encoding_quality	= json_find_first_label(root, "aac_encoding_quality");
	json_t *muxer_flavor			= json_find_first_label(root, "muxer_flavor");
	json_t *muxer_streams			= json_find_first_label(root, "muxer_streams");

	req->opts->muxer_flavor = muxer_flavor?atoi(muxer_flavor->child->text):MUXING_FLAVOR_QUICKTIME;
	char *extension;
	if (req->opts->muxer_flavor)
		extension = "mp4";
	else
		extension = "3gp";

	char timestamp[16];
	get_timestamp_string(timestamp);

	sprintf(req->opts->file, "%s/precorder_%s.%s", DEFAULT_FILE_LOCATION, timestamp, extension);

	req->opts->data_throughput		= data_throughput?atoi(data_throughput->child->text):1;

	req->opts->num_buffers			= num_buffers?atoi(num_buffers->child->text):-1;
	req->opts->video_format			= video_format?atoi(video_format->child->text):VIDEO_FORMAT_H264;
	req->opts->video_bitrate		= video_bitrate?atoi(video_bitrate->child->text):64000;

	req->opts->audio_sampling_rate	= audio_sampling_rate?atoi(audio_sampling_rate->child->text):22050;
	req->opts->audio_encoding		= audio_encoding?atoi(audio_encoding->child->text):AUDIO_ENCODING_AAC;
	req->opts->aac_stream_bitrate	= aac_stream_bitrate?atoi(aac_stream_bitrate->child->text):128000;
	req->opts->aac_encoding_quality	= aac_encoding_quality?atoi(aac_encoding_quality->child->text):AAC_ENCODING_QUALITY_0;

	req->opts->muxer_streams		= muxer_streams?atoi(muxer_streams->child->text):MUXER_STREAMS_BOTH;

	json_free_value(&root);

	pthread_t record_thread;
	pthread_create(&record_thread, NULL, record_video_wrapper, req);

	return TRUE;

}

bool stop_record(LSHandle* lshandle, LSMessage *message, void *ctx) {

	LSError lserror;
	LSErrorInit(&lserror);

	bool ret = stop_recording();

	if (ret)
		LSMessageReply(lshandle, message, "{\"returnValue\":true}", &lserror);
	else
		LSMessageReply(lshandle, message, "{\"returnValue\":false}", &lserror);

	if (LSErrorIsSet(&lserror)) {
		LSErrorPrint(&lserror, stderr);
		LSErrorFree(&lserror);
	}

	return TRUE;

}


bool get_events(LSHandle* lshandle, LSMessage *message, void *ctx) {

	LSError lserror;
	LSErrorInit(&lserror);

	bool subscribed = false;
	LSSubscriptionProcess(lshandle, message, &subscribed, &lserror);

	if (!subscribed)
		LSMessageReply(lshandle, message, "{\"returnValue\":false,\"errorText\":\"Requires subscription.\"}", &lserror);

	if (LSErrorIsSet(&lserror)) {
		LSErrorPrint(&lserror, stderr);
		LSErrorFree(&lserror);
	}

	return TRUE;

}

LSMethod methods[] = {
		{"set_led",			set_led},
		{"start_record",	start_record},
		{"stop_record",		stop_record},
		{"get_events",		get_events},
		{0,0}
};

void start_service() {

	LSError lserror;
	LSErrorInit(&lserror);

	GMainLoop *loop = g_main_loop_new(NULL, FALSE);

	LSRegisterPalmService(SERVICE_URI, &serviceHandle, &lserror);
	LSPalmServiceRegisterCategory(serviceHandle, "/", methods, NULL, NULL, NULL, &lserror);
	LSGmainAttachPalmService(serviceHandle, loop, &lserror);

	priv_bus = LSPalmServiceGetPrivateConnection(serviceHandle);
	pub_bus = LSPalmServiceGetPublicConnection(serviceHandle);

	g_main_loop_run(loop);

	if (LSErrorIsSet(&lserror)) {
		LSErrorPrint(&lserror, stderr);
		LSErrorFree(&lserror);
	}

}

void respond_to_gst_event(int message_type, char *message) {

	LSError lserror;
	LSErrorInit(&lserror);

	int len = 0;
	char *jsonResponse = 0;

	len = asprintf(&jsonResponse, "{\"gst_message_type\":%d,\"message\":\"%s\"}", message_type, message);

	LSSubscriptionRespond(serviceHandle, "/get_events", jsonResponse, &lserror);

	if (LSErrorIsSet(&lserror)) {
		LSErrorPrint(&lserror, stderr);
		LSErrorFree(&lserror);
	}

}
