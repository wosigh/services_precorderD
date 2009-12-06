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

#include "TPS6105X.h"
#include "misc.h"

static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data) {

	GMainLoop *loop = (GMainLoop *) data;

	switch (GST_MESSAGE_TYPE (msg)) {

	case GST_MESSAGE_EOS:
		g_print ("End of stream\n");
		g_main_loop_quit (loop);
		break;

	case GST_MESSAGE_ERROR: {
		gchar  *debug;
		GError *error;

		gst_message_parse_error (msg, &error, &debug);
		g_free (debug);

		g_printerr ("Error: %s\n", error->message);
		g_error_free (error);

		g_main_loop_quit (loop);
		break;
	}
	default:
		break;
	}

	return TRUE;

}

int main(int argc, char *argv[]) {

	GMainLoop *loop;

	GstElement *pipeline, *camsrc, *alsasrc, *vencoder, *aencoder, *muxer, *queue;
	GstBus *bus;

	gst_init(NULL, NULL);

	loop = g_main_loop_new(NULL, FALSE);

	pipeline = gst_pipeline_new("precorder");

	camsrc = gst_element_factory_make("camsrc", "camera-source");
	g_object_set(G_OBJECT(camsrc), "num-buffers", 300, NULL);

	alsasrc = gst_element_factory_make("alsasrc", "alsa-source");

	muxer = gst_element_factory_make("palmmpeg4mux", "palm-muxer");
	g_object_set(G_OBJECT(muxer), "name", "mux", NULL);
	g_object_set(G_OBJECT(muxer), "QTQCELPMuxing", 1, NULL);
	g_object_set(G_OBJECT(muxer), "StreamMuxSelection", 1, NULL);
	g_object_set(G_OBJECT(muxer), "enable", 1, NULL);

	vencoder = gst_element_factory_make("palmvideoencoder",	"palm-video-encoder");
	g_object_set(G_OBJECT(vencoder), "videoformat", 2, NULL);

	aencoder = gst_element_factory_make("palmaudioencoder", "palm-audio-encoder");
	g_object_set(G_OBJECT(aencoder), "encoding", 2, NULL);
	g_object_set(G_OBJECT(aencoder), "enable", 1, NULL);

	queue = gst_element_factory_make("queue", "queue");

	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);

	gst_bin_add_many(GST_BIN(pipeline), camsrc, vencoder, muxer, NULL);

	gst_element_link_many(camsrc, vencoder, muxer, NULL);
	//gst_element_link_many(alsasrc, queue, aencoder, NULL);
	//g_signal_connect (muxer, "mux", NULL, alsasrc);

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	g_print ("Running...\n");
	g_main_loop_run (loop);

	g_print ("Returned, stopping playback\n");
	gst_element_set_state (pipeline, GST_STATE_NULL);

	g_print ("Deleting pipeline\n");
	gst_object_unref (GST_OBJECT (pipeline));

	return 0;

}
