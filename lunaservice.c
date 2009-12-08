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
#include <lunaservice.h>

#include "gstreamer.h"
#include "TPS6105X.h"

#define SERVICE_URI				"us.ryanhope.precorderD"
#define DEFAULT_FILE_LOCATION	"/media/internal/video"

LSPalmService *serviceHandle;
LSHandle *priv_bus;
LSHandle *pub_bus;

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

	PIPELINE_OPTS_t *opts =  malloc(sizeof(PIPELINE_OPTS_t));

	LSError lserror;
	LSErrorInit(&lserror);

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

	opts->muxer_flavor = muxer_flavor?atoi(muxer_flavor->child->text):MUXING_FLAVOR_QUICKTIME;
	char *extension;
	if (opts->muxer_flavor)
		extension = "mp4";
	else
		extension = "3gp";

	char timestamp[16];
	get_timestamp_string(timestamp);

	sprintf(opts->file, "%s/precorder_%s.%s", DEFAULT_FILE_LOCATION, timestamp, extension);

	opts->data_throughput		= data_throughput?atoi(data_throughput->child->text):1;

	opts->num_buffers			= num_buffers?atoi(num_buffers->child->text):150;
	opts->video_format			= video_format?atoi(video_format->child->text):VIDEO_FORMAT_H264;
	opts->video_bitrate			= video_bitrate?atoi(video_bitrate->child->text):64000;

	opts->audio_sampling_rate	= audio_sampling_rate?atoi(audio_sampling_rate->child->text):22050;
	opts->audio_encoding		= audio_encoding?atoi(audio_encoding->child->text):AUDIO_ENCODING_AAC;
	opts->aac_stream_bitrate	= aac_stream_bitrate?atoi(aac_stream_bitrate->child->text):128000;
	opts->aac_encoding_quality	= aac_encoding_quality?atoi(aac_encoding_quality->child->text):AAC_ENCODING_QUALITY_5;

	opts->muxer_streams			= muxer_streams?atoi(muxer_streams->child->text):MUXER_STREAMS_BOTH;

	json_free_value(&root);

	pthread_t record_thread;
	pthread_create(&record_thread, NULL, record_video, opts);

	//pthread_join(record_thread, NULL);

	LSMessageReply(lshandle, message, "{\"returnValue\":true}", &lserror);

	end:
	if (LSErrorIsSet(&lserror)) {
		LSErrorPrint(&lserror, stderr);
		LSErrorFree(&lserror);
	}

	return TRUE;

}

LSMethod methods[] = {
		{"set_led",			set_led},
		{"start_record",	start_record},
		//{"stop_record",		stop_record},
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