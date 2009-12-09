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

	switch (GST_MESSAGE_TYPE (msg)) {

	case GST_MESSAGE_NEW_CLOCK: {
		GstClock *clock;
		gst_message_parse_new_clock (msg, &clock);
		g_print("New clock: %s\n", (clock ? GST_OBJECT_NAME (clock) : "NULL"));
		break;
	}

	case GST_MESSAGE_EOS:
		g_print("End of stream\n");
		g_main_loop_quit(recording_loop);
		break;

	case GST_MESSAGE_ERROR: {
		gchar *debug;
		GError *error;

		gst_message_parse_error(msg, &error, &debug);
		g_free(debug);

		g_printerr("Error: %s\n", error->message);
		g_error_free(error);

		g_main_loop_quit(recording_loop);
		break;
	}
	default:
		break;
	}

	return TRUE;

}

static gboolean bus_call2(GstBus *bus, GstMessage *msg, gpointer data) {

	switch (GST_MESSAGE_TYPE(msg)) {

	case GST_MESSAGE_UNKNOWN: 			g_print(">>>> GST_MESSAGE_UNKNOWN\n"); break;
	case GST_MESSAGE_EOS: 				g_print(">>>> GST_MESSAGE_EOS\n"); break;
	case GST_MESSAGE_ERROR:				g_print(">>>> GST_MESSAGE_ERROR\n"); break;
	case GST_MESSAGE_WARNING:			g_print(">>>> GST_MESSAGE_WARNING\n"); break;
	case GST_MESSAGE_INFO:				g_print(">>>> GST_MESSAGE_INFO\n"); break;
	case GST_MESSAGE_TAG:				g_print(">>>> GST_MESSAGE_TAG\n"); break;
	case GST_MESSAGE_BUFFERING:			g_print(">>>> GST_MESSAGE_BUFFERING\n"); break;
	case GST_MESSAGE_STATE_CHANGED:		g_print(">>>> GST_MESSAGE_STATE_CHANGED\n"); break;
	case GST_MESSAGE_STATE_DIRTY:		g_print(">>>> GST_MESSAGE_STATE_DIRTY\n"); break;
	case GST_MESSAGE_STEP_DONE:			g_print(">>>> GST_MESSAGE_STEP_DONE\n"); break;
	case GST_MESSAGE_CLOCK_PROVIDE:		g_print(">>>> GST_MESSAGE_CLOCK_PROVIDE\n"); break;
	case GST_MESSAGE_CLOCK_LOST:		g_print(">>>> GST_MESSAGE_CLOCK_LOST\n"); break;
	case GST_MESSAGE_NEW_CLOCK:			g_print(">>>> GST_MESSAGE_NEW_CLOCK\n"); break;
	case GST_MESSAGE_STRUCTURE_CHANGE:	g_print(">>>> GST_MESSAGE_STRUCTURE_CHANGE\n"); break;
	case GST_MESSAGE_STREAM_STATUS: 	g_print(">>>> GST_MESSAGE_STREAM_STATUS\n"); break;
	case GST_MESSAGE_APPLICATION:		g_print(">>>> GST_MESSAGE_APPLICATION\n"); g_main_loop_quit(recording_loop); break;
	case GST_MESSAGE_ELEMENT:			g_print(">>>> GST_MESSAGE_ELEMENT\n"); break;
	case GST_MESSAGE_SEGMENT_START:		g_print(">>>> GST_MESSAGE_SEGMENT_START\n"); break;
	case GST_MESSAGE_SEGMENT_DONE:		g_print(">>>> GST_MESSAGE_SEGMENT_DONE\n"); break;
	case GST_MESSAGE_DURATION:			g_print(">>>> GST_MESSAGE_DURATION\n"); break;
	case GST_MESSAGE_LATENCY: 			g_print(">>>> GST_MESSAGE_LATENCY\n"); break;
	case GST_MESSAGE_ASYNC_START:		g_print(">>>> GST_MESSAGE_ASYNC_START\n"); break;
	case GST_MESSAGE_ASYNC_DONE:		g_print(">>>> GST_MESSAGE_ASYNC_DONE\n"); break;

	}

	return TRUE;

}

void gst_object_deep_notify(GObject *object, GstObject *orig, GParamSpec *pspec, gchar **excluded_props) {

	GValue value = { 0, };        /* the important thing is that value.type = 0 */
	gchar *str = NULL;
	gchar *name = NULL;

	if (pspec->flags & G_PARAM_READABLE) {
		/* let's not print these out for excluded properties... */
		while (excluded_props != NULL && *excluded_props != NULL) {
			if (strcmp (pspec->name, *excluded_props) == 0)
				return;
			excluded_props++;
		}
		g_value_init (&value, G_PARAM_SPEC_VALUE_TYPE (pspec));
		g_object_get_property (G_OBJECT (orig), pspec->name, &value);

		/* FIXME: handle flags */
		if (G_IS_PARAM_SPEC_ENUM (pspec)) {
			GEnumValue *enum_value;

			enum_value =
				g_enum_get_value (G_ENUM_CLASS (g_type_class_ref (pspec->value_type)),
						g_value_get_enum (&value));

			str = g_strdup_printf ("%s (%d)", enum_value->value_nick,
					enum_value->value);
		} else {
			str = g_strdup_value_contents (&value);
		}
		name = gst_object_get_path_string (orig);
		g_print ("%s: %s = %s\n", name, pspec->name, str);
		g_free (name);
		g_free (str);
		g_value_unset (&value);
	} else {
		name = gst_object_get_path_string (orig);
		g_warning ("Parameter %s not readable in %s.", pspec->name, name);
		g_free (name);
	}

}

int record_video(PIPELINE_OPTS_t *opts) {

	int ret = -1;

	GstElement *pipeline, *vsrc, *asrc, *venc, *aenc, *vqueue, *aqueue, *muxer;
	GstCaps *vcaps, *acaps;
	GstBus *bus;

	gst_init(NULL, NULL);

	recording_loop = g_main_loop_new(NULL, FALSE);

	// Create pipeline
	pipeline = gst_pipeline_new("precorder");

	// Setup video source
	vsrc = gst_element_factory_make("camsrc", "video-source");
	g_object_set(G_OBJECT(vsrc), "num-buffers", opts->num_buffers, NULL);
	g_object_set(G_OBJECT(vsrc), "queue-size", 6, NULL);
	g_object_set(G_OBJECT(vsrc), "do-timestamp", 1, NULL);
	g_object_set(G_OBJECT(vsrc), "typefind", 1, NULL);

	// Setup video queue
	vqueue = gst_element_factory_make("queue", "video-queue");

	// Setup video encoder
	venc = gst_element_factory_make("palmvideoencoder", "video-encoder");
	g_object_set(G_OBJECT(venc), "videoformat", opts->video_format, NULL);
	//g_object_set(G_OBJECT(venc), "enable", opts->data_throughput, NULL);

	// Setup audio source
	asrc = gst_element_factory_make("alsasrc", "audio-source");
	//g_object_set(G_OBJECT(asrc), "slave-method", 0, NULL);

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
	g_object_set(G_OBJECT(muxer), "location", opts->file, NULL);
	g_object_set(G_OBJECT(muxer), "QTQCELPMuxing", opts->muxer_flavor, NULL);
	g_object_set(G_OBJECT(muxer), "StreamMuxSelection", opts->muxer_streams, NULL);
	g_object_set(G_OBJECT(muxer), "enable", opts->data_throughput, NULL);

	// Bundle up elements into a bin
	gst_bin_add_many(GST_BIN(pipeline), vsrc, vqueue, venc, asrc, aqueue, aenc, muxer, NULL);

	// Build video caps
	vcaps = gst_caps_new_simple(
			"video/x-raw-yuv",
			"width",			G_TYPE_INT,			480,
			"height",			G_TYPE_INT,			320,
			"framerate",		GST_TYPE_FRACTION,	30, 1,
			//"format", GST_TYPE_FOURCC, GST_MAKE_FOURCC ('U', 'Y', 'V', 'Y'),
			NULL
	);

	// Build audio caps
	acaps = gst_caps_new_simple(
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

	gst_element_link(vsrc, venc);
	gst_element_link_many(venc, muxer, NULL);

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_watch(bus, bus_call2, recording_loop);

	g_signal_connect(pipeline, "deep_notify", G_CALLBACK(gst_object_default_deep_notify), NULL);

	gst_object_unref(bus);

	g_main_loop_run(recording_loop);

	gst_element_set_state (pipeline, GST_STATE_NULL);

	gst_object_unref (GST_OBJECT (pipeline));

	ret = 0;

	return ret;

}
