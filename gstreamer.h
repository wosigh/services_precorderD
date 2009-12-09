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
 * \brief Possible video formats for the
 * 'palmvideoencoder' gst element
 */
typedef enum {
	VIDEO_FORMAT_MPEG4,		//!< MPEG-4
	VIDEO_FORMAT_H263,		//!< H.263
	VIDEO_FORMAT_H264,		//!< H.264(AVC)
} VIDEO_FORMAT_t;

/*!
 * \brief Possible audio encoding for the
 * 'palmaudioencoder' gst element
 */
typedef enum {
	AUDIO_ENCODING_AAC,		//!< AAC
	AUDIO_ENCODING_AMRNB,	//!< AMRNB
	AUDIO_ENCODING_MP3,		//!< MP3
} AUDIO_ENCODING_t;

/*!
 * \brief Possible AAC encoding quality values for the
 * 'palmaudioencoder' gst element
 */
typedef enum {
	AAC_ENCODING_QUALITY_0,		//!< CBR
	AAC_ENCODING_QUALITY_1,		//!< VBR-1 (lowest)
	AAC_ENCODING_QUALITY_2,		//!< VBR-2
	AAC_ENCODING_QUALITY_3,		//!< VBR-3
	AAC_ENCODING_QUALITY_4,		//!< VBR-4
	AAC_ENCODING_QUALITY_5,		//!< VBR-5 (highest)
} AACEncodingQuality_t;

typedef enum {
	MUXING_FLAVOR_3GP,			//!< 3GP/3G2 QCELP muxing
	MUXING_FLAVOR_QUICKTIME,	//!< Quicktime QCELP muxing
} MUXING_FLAVOR_t;

typedef enum {
	MUXER_STREAMS_BOTH,			//!< Both video and audio streams
	MUXER_STREAMS_VIDEO,		//!< Only video stream
	MUXER_STREAMS_AUDIO,		//!< Only audio stream
} MUXER_STREAMS_t;

/*!
 * \brief Video recording options
 */
typedef struct {

	char					file[PATH_MAX];

	unsigned int			data_throughput;

	int						num_buffers;
	VIDEO_FORMAT_t			video_format;
	unsigned int			video_bitrate;

	unsigned int			audio_sampling_rate;
	AUDIO_ENCODING_t		audio_encoding;
	unsigned int			aac_stream_bitrate;
	AACEncodingQuality_t	aac_encoding_quality;

	MUXING_FLAVOR_t			muxer_flavor;
	MUXER_STREAMS_t			muxer_streams;

} PIPELINE_OPTS_t;

GMainLoop *recording_loop;

int record_video(PIPELINE_OPTS_t *opts);

#endif /* GSTREAMER_H_ */
