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

#include <stdio.h>
#include <gst/gst.h>

#include "gstreamer.h"

static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data) {

	GMainLoop *loop = (GMainLoop *)data;

	switch (GST_MESSAGE_TYPE (msg)) {

	case GST_MESSAGE_NEW_CLOCK: {
		GstClock *clock;
		gst_message_parse_new_clock (msg, &clock);
		g_print("New clock: %s\n", (clock ? GST_OBJECT_NAME (clock) : "NULL"));
		break;
	}

	case GST_MESSAGE_EOS:
		g_print("End of stream\n");
		g_main_loop_quit (loop);
		break;

	case GST_MESSAGE_ERROR: {
		gchar *debug;
		GError *error;

		gst_message_parse_error(msg, &error, &debug);
		g_free(debug);

		g_printerr("Error: %s\n", error->message);
		g_error_free(error);

		g_main_loop_quit (loop);
		break;
	}
	default:
		break;
	}

	return TRUE;

}

void record_video(PIPELINE_OPTS_t *opts) {

	GMainLoop *loop;

	GstElement *pipeline, *vsrc, *asrc, *venc, *aenc, *vqueue, *aqueue, *muxer;
	GstBus *bus;

	gst_init(NULL, NULL);

	loop = g_main_loop_new(NULL, FALSE);

	// Create pipeline
	pipeline = gst_pipeline_new("precorder");

	// Setup video source
	vsrc = gst_element_factory_make("camsrc", "video-source");
	g_object_set(G_OBJECT(vsrc), "num-buffers", opts->num_buffers, NULL);

	// Setup video queue
	vqueue = gst_element_factory_make("queue", "video-queue");

	// Setup video encoder
	venc = gst_element_factory_make("palmvideoencoder", "video-encoder");
	g_object_set(G_OBJECT(venc), "videoformat", opts->video_format, NULL);
	g_object_set(G_OBJECT(venc), "enable", opts->data_throughput, NULL);

	// Setup audio source
	asrc = gst_element_factory_make("alsasrc", "audio-source");

	// Setup audio queue
	aqueue = gst_element_factory_make("queue", "audio-queue");

	// Setup audio encoder
	aenc = gst_element_factory_make("palmaudioencoder", "audio-encoder");
	g_object_set(G_OBJECT(aenc), "encoding", opts->audio_encoding, NULL);
	g_object_set(G_OBJECT(aenc), "AACStreamBitrate", opts->aac_stream_bitrate, NULL);
	g_object_set(G_OBJECT(aenc), "AACEncodingQuality", opts->aac_encoding_quality, NULL);
	g_object_set(G_OBJECT(aenc), "enable", opts->data_throughput, NULL);

	// Setip muxer
	muxer = gst_element_factory_make("palmmpeg4mux", "muxer");
	g_object_set(G_OBJECT(muxer), "QTQCELPMuxing", opts->muxer_flavor, NULL);
	g_object_set(G_OBJECT(muxer), "StreamMuxSelection", opts->muxer_streams, NULL);
	g_object_set(G_OBJECT(muxer), "enable", opts->data_throughput, NULL);

	// Bundle up elements into a bin
	gst_bin_add_many(GST_BIN(pipeline), vsrc, vqueue, venc, asrc, aqueue, aenc, muxer, NULL);

	// Build audio caps
	GstCaps *acaps = gst_caps_new_simple(
			"audio/x-raw-int",
			"width",			G_TYPE_INT,		16,
			"depth",			G_TYPE_INT,		16,
			"endianness",		G_TYPE_INT,		1234,
			"rate",				G_TYPE_INT,		opts->audio_sampling_rate,
			"channels",			G_TYPE_INT,		2,
			"signed",			G_TYPE_BOOLEAN,	TRUE,
			NULL
	);

	// Link elements
	gst_element_link_filtered(asrc, aqueue, acaps);
	gst_element_link_many(aqueue, aenc, muxer, NULL);
	gst_element_link_many(vsrc, venc, muxer, NULL);

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);

	g_main_loop_run (loop);

	gst_element_set_state (pipeline, GST_STATE_NULL);

	gst_object_unref (GST_OBJECT (pipeline));

}
