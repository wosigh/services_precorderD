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

#ifndef GSTREAMER_H_
#define GSTREAMER_H_

#include <limits.h>
#include <stdbool.h>
#include <pthread.h>
#include <glib.h>

/*!
 * \brief Possible source devices for
 * 'pulsesrc' gst element
 */
typedef enum {
	SOURCE_DEVICE_MIC,		//!< pcm_input
	SOURCE_DEVICE_STREAM,	//!< pcm_output.monitor
} SOURCE_DEVICE_t;



typedef enum {
	VOICE_ACTIVATION_YES,	//!< Limit gstlevel
	VOICE_ACTIVATION_NO,	//!< Unlimit gstlevel
} VOICE_ACTIVATION_t;


/*!
 * \brief recording options
 */
typedef struct {

	char					file[PATH_MAX];

	SOURCE_DEVICE_t			source_device;

	unsigned int			stream_rate;
	unsigned int			channels;
	unsigned int			endianness;

	unsigned int			width;
	unsigned int			depth;
	unsigned int			lame_bitrate;
	unsigned int			lame_quality;

	char					filename;
	VOICE_ACTIVATION_t		voice_activation;

} PIPELINE_OPTS_t;

GMainLoop *recording_loop;

int record_video(PIPELINE_OPTS_t *opts);
bool stop_recording();

#endif /* GSTREAMER_H_ */
