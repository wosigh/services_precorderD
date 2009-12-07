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

#include <stdbool.h>

#include "gstreamer.h"

int main(int argc, char *argv[]) {

	PIPELINE_OPTS_t opts;

	opts.data_throughput		= true;

	opts.num_buffers			= 300;
	opts.video_format			= VIDEO_FORMAT_H264;
	opts.video_bitrate			= 64000;

	opts.audio_sampling_rate	= 22050;
	opts.audio_encoding			= AUDIO_ENCODING_AAC;
	opts.aac_stream_bitrate		= 128000;
	opts.aac_encoding_quality	= AAC_ENCODING_QUALITY_0;

	opts.muxer_flavor			= MUXING_FLAVOR_QUICKTIME;
	opts.muxer_streams			= MUXER_STREAMS_BOTH;

	record_video(&opts);

	return 0;

}
