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
#include <string.h>

#include "TPS6105X.h"

/*!
 * \brief A getter/setter method for the TPS6105X avin sysfs interface.
 *
 *
 * \param value <0 to read, =0 to disable, >0 to enable
 *
 * \return <0 for error, =0 for disabled, >0 for enabled
 */
int TPS6105X_avin(int value) {

	FILE *fp;
	int len = 0, state = -1;

	if (value < 0) goto read;

	fp = fopen(PATH_TPS6105X_AVIN, "w");
	if (fp) {
		if (value>0) {
			fprintf(fp, "1");
		} else {
			fprintf(fp, "0");
		}
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

/*!
 * \brief A getter/setter method for the TPS6105X 'torch_current' sysfs interface.
 *
 *
 * \param value <0 to read, >=0 to set value
 *
 * \return <0 for error, >=0 for the current value of the sysfs interface
 */
TPS6105X_TORCH_CURRENT_t TPS6105X_torch_current(TPS6105X_TORCH_CURRENT_t value) {

	FILE *fp;

	if (value < 0) goto read;

	fp = fopen(PATH_TPS6105X_TORCH_CURRENT, "w");
	if (fp) {
		if (value==TPS6105X_TORCH_CURRENT_0)
			fprintf(fp, TPS6105X_TORCH_CURRENT__0);
		else if (value==TPS6105X_TORCH_CURRENT_1)
			fprintf(fp, TPS6105X_TORCH_CURRENT__1);
		else if (value==TPS6105X_TORCH_CURRENT_2)
			fprintf(fp, TPS6105X_TORCH_CURRENT__2);
		else if (value==TPS6105X_TORCH_CURRENT_3)
			fprintf(fp, TPS6105X_TORCH_CURRENT__3);
		else if (value==TPS6105X_TORCH_CURRENT_4)
			fprintf(fp, TPS6105X_TORCH_CURRENT__4);
		else if (value==TPS6105X_TORCH_CURRENT_5)
			fprintf(fp, TPS6105X_TORCH_CURRENT__5);
		else if (value==TPS6105X_TORCH_CURRENT_6)
			fprintf(fp, TPS6105X_TORCH_CURRENT__6);
		else if (value==TPS6105X_TORCH_CURRENT_7)
			fprintf(fp, TPS6105X_TORCH_CURRENT__7);
		fflush(fp);
		close(fp);
	}

	read:

	fp = fopen(PATH_TPS6105X_TORCH_CURRENT, "r");
	if (fp) {
		int i = 0, len = 0;
		char values[TPS6105X_TORCH_CURRENT_NUM][TPS6105X__MAX_VALUE_LENGTH];
		len = fscanf(fp, "%s%s%s%s%s%s%s%s", values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7], values[8]);
		for (;i<TPS6105X_TORCH_CURRENT_NUM;i++) {
			len = strlen(values[i]);
			if (values[i][len-1] == '*') {
				return i;
			}
		}
		fclose(fp);
	}

	return -1;

}

/*!
 * \brief A getter/setter method for the TPS6105X 'flash_current' sysfs interface.
 *
 *
 * \param value <0 to read, >=0 to set value
 *
 * \return <0 for error, >=0 for the current value of the sysfs interface
 */
TPS6105X_FLASH_CURRENT_t TPS6105X_flash_current(TPS6105X_FLASH_CURRENT_t value) {

	FILE *fp;

	if (value < 0) goto read;

	fp = fopen(PATH_TPS6105X_FLASH_CURRENT, "w");
	if (fp) {
		if (value==TPS6105X_FLASH_CURRENT_0)
			fprintf(fp, TPS6105X_FLASH_CURRENT__0);
		else if (value==TPS6105X_FLASH_CURRENT_1)
			fprintf(fp, TPS6105X_FLASH_CURRENT__1);
		else if (value==TPS6105X_FLASH_CURRENT_2)
			fprintf(fp, TPS6105X_FLASH_CURRENT__2);
		else if (value==TPS6105X_FLASH_CURRENT_3)
			fprintf(fp, TPS6105X_FLASH_CURRENT__3);
		else if (value==TPS6105X_FLASH_CURRENT_4)
			fprintf(fp, TPS6105X_FLASH_CURRENT__4);
		else if (value==TPS6105X_FLASH_CURRENT_5)
			fprintf(fp, TPS6105X_FLASH_CURRENT__5);
		else if (value==TPS6105X_FLASH_CURRENT_6)
			fprintf(fp, TPS6105X_FLASH_CURRENT__6);
		else if (value==TPS6105X_FLASH_CURRENT_7)
			fprintf(fp, TPS6105X_FLASH_CURRENT__7);
		fflush(fp);
		close(fp);
	}

	read:

	fp = fopen(PATH_TPS6105X_FLASH_CURRENT, "r");
	if (fp) {
		int i = 0, len = 0;
		char values[TPS6105X_FLASH_CURRENT_NUM][TPS6105X__MAX_VALUE_LENGTH];
		len = fscanf(fp, "%s%s%s%s%s%s%s%s", values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7], values[8]);
		for (;i<TPS6105X_FLASH_CURRENT_NUM;i++) {
			len = strlen(values[i]);
			if (values[i][len-1] == '*') {
				return i;
			}
		}
		fclose(fp);
	}

	return -1;

}

/*!
 * \brief A getter/setter method for the TPS6105X 'mode' sysfs interface.
 *
 *
 * \param value <0 to read, >=0 to set value
 *
 * \return <0 for error, >=0 for the current value of the sysfs interface
 */
TPS6105X_MODE_t TPS6105X_mode(TPS6105X_MODE_t value) {

	FILE *fp;

	if (value < 0) goto read;

	fp = fopen(PATH_TPS6105X_MODE, "w");
	if (fp) {
		if (value==TPS6105X_MODE_0)
			fprintf(fp, TPS6105X_MODE__0);
		else if (value==TPS6105X_MODE_1)
			fprintf(fp, TPS6105X_MODE__1);
		else if (value==TPS6105X_MODE_2)
			fprintf(fp, TPS6105X_MODE__2);
		fflush(fp);
		close(fp);
	}

	read:

	fp = fopen(PATH_TPS6105X_MODE, "r");
	if (fp) {
		int i = 0, len = 0;
		char values[TPS6105X_MODE_NUM][TPS6105X__MAX_VALUE_LENGTH];
		len = fscanf(fp, "%s%s%s", values[0], values[1], values[2]);
		for (;i<TPS6105X_MODE_NUM;i++) {
			len = strlen(values[i]);
			if (values[i][len-1] == '*') {
				return i;
			}
		}
		fclose(fp);
	}

	return -1;

}
