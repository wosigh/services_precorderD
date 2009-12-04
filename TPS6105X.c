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

#define PATH_TPS6105X_AVIN				"/sys/class/i2c-adapter/i2c-2/2-0033/avin"
#define PATH_TPS6105X_MODE				"/sys/class/i2c-adapter/i2c-2/2-0033/mode"
#define PATH_TPS6105X_FLASH_CURRENT		"/sys/class/i2c-adapter/i2c-2/2-0033/flash_current"
#define PATH_TPS6105X_TORCH_CURRENT		"/sys/class/i2c-adapter/i2c-2/2-0033/torch_current"

#include <stdlib.h>
#include <stdio.h>

#include "TPS6105X.h"

/**
 * This is a getter/setter method for the TPS6105X avin sysfs interface.
 *
 * @param value <0 to read, =0 to disable, >0 to enable
 * @return <0 for error, =0 for disabled, >0 for enabled
 */
int TPS6105X_avin(int value) {

	FILE *fp;
	int len = 0, state = -1;

	if (value < 0) goto read;

	fp = fopen(PATH_TPS6105X_AVIN, "w");
	if (fp) {
		if (value>0)
			fprintf(fp, "0");
		else
			fprintf(fp, "1");
		fflush(fp);
		close(fp);
	}

	read:

	fp = fopen(PATH_TPS6105X_AVIN, "r");
	if (fp) {
		state = fgetc(fp);
		fclose(fp);
	}

	return state;

}

void TPS6105X_mode() {

}
