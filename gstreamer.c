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
#include <math.h>
#include <gst/gst.h>

#include "gstreamer.h"
#include "luna.h"

GstElement *pipeline;
gdouble rms;

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {

	bool quit_recording_loop = FALSE;

	int len = 0;
	char *message = 0;

	int message_type = GST_MESSAGE_TYPE(msg);

	switch (message_type) {

	case GST_MESSAGE_UNKNOWN:
		break;
	case GST_MESSAGE_EOS:
		break;
	case GST_MESSAGE_ERROR: {
		gchar *debug;
		GError *error;
		gst_message_parse_error(msg, &error, &debug);
		g_free(debug);
		asprintf(&message, "Error: %s", error->message);
		g_error_free(error);
		break;
	}
	case GST_MESSAGE_WARNING:
		break;
	case GST_MESSAGE_INFO:
		break;
	case GST_MESSAGE_TAG: {
		int idx = 0;
		GstTagList *taglist;
		gst_message_parse_tag(msg, &taglist);
		asprintf(&message, "%s", gst_structure_to_string(taglist));
		gst_tag_list_free(taglist);
		break;
	}
	case GST_MESSAGE_BUFFERING:
		break;
	case GST_MESSAGE_STATE_CHANGED: {
		GstState *oldState, *newState, *pendingState;
		gst_message_parse_state_changed(msg, oldState, newState, pendingState);
		asprintf(&message, "Old state: %d, New state: %d, Pending state: %d", oldState, newState, pendingState);
		break;
	}
	case GST_MESSAGE_STATE_DIRTY:
		break;
	case GST_MESSAGE_STEP_DONE:
		break;
	case GST_MESSAGE_CLOCK_PROVIDE: {
		GstClock *clock;
		gboolean *ready;
		gst_message_parse_clock_provide(msg, &clock, ready);
		asprintf(&message, "Provide clock: %s, Ready: %d", (clock ? GST_OBJECT_NAME(clock) : "NULL"), ready);
		break;
	}
	case GST_MESSAGE_CLOCK_LOST: {
		GstClock *clock;
		gst_message_parse_clock_lost(msg, &clock);
		asprintf(&message, "Lost clock: %s", (clock ? GST_OBJECT_NAME(clock) : "NULL"));
		break;
	}
	case GST_MESSAGE_NEW_CLOCK: {
		GstClock *clock;
		gst_message_parse_new_clock(msg, &clock);
		asprintf(&message, "New clock: %s", (clock ? GST_OBJECT_NAME(clock) : "NULL"));
		break;
	}
	case GST_MESSAGE_STRUCTURE_CHANGE:
		break;
	case GST_MESSAGE_STREAM_STATUS:
		break;
	case GST_MESSAGE_APPLICATION:
		if (message_type==16384)
			quit_recording_loop = TRUE;
		break;
	case GST_MESSAGE_ELEMENT:
		break;
	case GST_MESSAGE_SEGMENT_START:
		break;
	case GST_MESSAGE_SEGMENT_DONE:
		break;
	case GST_MESSAGE_DURATION:
		break;
	case GST_MESSAGE_LATENCY:
		break;
	case GST_MESSAGE_ASYNC_START:
		break;
	case GST_MESSAGE_ASYNC_DONE:
		break;

	}

	respond_to_gst_event(message_type, message);

	if (quit_recording_loop)
		g_main_loop_quit(recording_loop);

	if (message) free(message);

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

gboolean message_handler (GstBus * bus, GstMessage * message, gpointer data)
{
  // handle only element messages
  if (message->type == GST_MESSAGE_ELEMENT) {
    const GstStructure *s = gst_message_get_structure (message);
    const gchar *name = gst_structure_get_name (s);
    // handle only level element messages
    if (strcmp (name, "level") == 0) {
      gint channels;
      gdouble rms_dB;
      const GValue *list;
      const GValue *value;

      gint i;

      list = gst_structure_get_value (s, "rms");
      channels = gst_value_list_get_size (list);

      for (i = 0; i < channels; ++i) {
        list = gst_structure_get_value (s, "rms");
        value = gst_value_list_get_value (list, i);
        rms_dB = g_value_get_double (value);

        // normalize rms value, results in range from 0.0 to 1.0
        rms = pow (10, rms_dB / 20);
      }
    }
  }
  /* we handled the message we want, and ignored the ones we didn't want.
   * so the core can unref the message for us */
  return TRUE;
}



int record_video(PIPELINE_OPTS_t *opts) {

	int ret = -1;

	GstElement *psrc, *vact, *aenc, *fsink;
	GstCaps *acaps;
	GstBus *bus;
	gint watch_id;
	extern gdouble rms;

	gst_init(NULL, NULL);

	recording_loop = g_main_loop_new(NULL, FALSE);

	// Create pipeline
	pipeline = gst_pipeline_new("precorder");

	// Setup pulse source
	psrc = gst_element_factory_make("pulsesrc", "pulse-source");
	g_object_set(G_OBJECT(psrc), "device", opts->source_device, NULL);

	// Setup audio encoder
	aenc = gst_element_factory_make("lame", "audio-encoder");
	g_object_set(G_OBJECT(aenc), "bitrate", opts->lame_bitrate, NULL);
	g_object_set(G_OBJECT(aenc), "quality", opts->lame_quality, NULL);

	// Setup file sink
	fsink = gst_element_factory_make("filesink", "file-sink");
	g_object_set(G_OBJECT(fsink), "location", opts->file, NULL);

	// Setup voice activation (level checker) and turn on message output
	vact = gst_element_factory_make("level", "voice-activation");
	g_object_set (G_OBJECT (vact), "message", TRUE, NULL);

	// Bundle up elements into a bin
	gst_bin_add_many(GST_BIN(pipeline), psrc, vact, aenc, fsink, NULL);

	// Build audio caps
	acaps = gst_caps_new_simple(
			"audio/x-raw-int",
			"width",			G_TYPE_INT,		opts->width,
			"depth",			G_TYPE_INT,		opts->depth,
			"endianness",		G_TYPE_INT,		opts->endianness,
			"rate",				G_TYPE_INT,		opts->stream_rate,
			"channels",			G_TYPE_INT,		opts->channels,
			"signed",			G_TYPE_BOOLEAN,	TRUE,
			NULL
	);

	// Link elements
	gst_element_link_filtered(psrc, vact, aenc);
	gst_element_link(fsink, NULL);

	bus = gst_element_get_bus (precorder);
	watch_id = gst_bus_add_watch (bus, message_handler, NULL);

	int state;
	state = 1;

	if (opts->voice_activation == VOICE_ACTIVATION_YES) {
		// run sync'ed so it doesn't trip over itself
		g_object_set (G_OBJECT (filesink), "sync", TRUE, NULL);
		while (gst_app_sink_is_eos() == FALSE) {
			if ((rms >= 0.2) && (state != 1)) {
				gst_element_set_state(pipeline, GST_STATE_PLAYING);
				state = 1;
			}
			else if ((rms <= 0.2) && (state != 0)) {
				gst_element_set_state(pipeline, GST_STATE_PAUSED);
				state = 0;
			}
		}
	}
	else {
		gst_element_set_state(pipeline, GST_STATE_PLAYING);
	}


	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_watch(bus, bus_call, recording_loop);

	//g_signal_connect(pipeline, "deep_notify", G_CALLBACK(gst_object_default_deep_notify), NULL);

	gst_object_unref(bus);

	g_main_loop_run(recording_loop);

	gst_element_set_state(pipeline, GST_STATE_NULL);

	gst_object_unref(GST_OBJECT (pipeline));

	ret = 0;

	return ret;

}

bool stop_recording() {

	return gst_element_send_event(pipeline, gst_event_new_eos());

}
