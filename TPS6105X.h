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

#ifndef TPS6105X_H_
#define TPS6105X_H_

typedef enum {
	TPS6105X_CURRENT_0,
#define TPS6105X_CURRENT__0	"0mA"
	TPS6105X_CURRENT_1,
#define TPS6105X_CURRENT__1	"50mA"
	TPS6105X_CURRENT_2,
#define TPS6105X_CURRENT__2	"75mA"
	TPS6105X_CURRENT_3,
#define TPS6105X_CURRENT__3	"100mA"
	TPS6105X_CURRENT_4,
#define TPS6105X_CURRENT__4	"150mA"
	TPS6105X_CURRENT_5,
#define TPS6105X_CURRENT__5	"200mA"
	TPS6105X_CURRENT_6,
#define TPS6105X_CURRENT__6	"250/400mA"
	TPS6105X_CURRENT_7,
#define TPS6105X_CURRENT__7	"250/500mA"
	TPS6105X_CURRENT_NUM,
} TPS6105X_CURRENT_t;

int TPS6105X_avin(int value);

#endif /* TPS6105X_H_ */
