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

#define TPS6105X__MAX_VALUE_LENGTH	16

/*!
 * \brief Possible torch current values
 */
typedef enum {
	TPS6105X_TORCH_CURRENT_0,				//!< 0mA
	TPS6105X_TORCH_CURRENT_1,				//!< 50mA
	TPS6105X_TORCH_CURRENT_2,				//!< 75mA
	TPS6105X_TORCH_CURRENT_3,				//!< 100mA
	TPS6105X_TORCH_CURRENT_4,				//!< 150mA
	TPS6105X_TORCH_CURRENT_5,				//!< 200mA
	TPS6105X_TORCH_CURRENT_6,				//!< 250/400mA
	TPS6105X_TORCH_CURRENT_7,				//!< 250/500mA
	TPS6105X_TORCH_CURRENT_NUM,
} TPS6105X_TORCH_CURRENT_t;

#define TPS6105X_TORCH_CURRENT__0			"0mA"
#define TPS6105X_TORCH_CURRENT__1			"50mA"
#define TPS6105X_TORCH_CURRENT__2			"75mA"
#define TPS6105X_TORCH_CURRENT__3			"100mA"
#define TPS6105X_TORCH_CURRENT__4			"150mA"
#define TPS6105X_TORCH_CURRENT__5			"200mA"
#define TPS6105X_TORCH_CURRENT__6			"250/400mA"
#define TPS6105X_TORCH_CURRENT__7			"250/500mA"

/*!
 * \brief Possible flash current values
 */
typedef enum {
	TPS6105X_FLASH_CURRENT_0,				//!< 150mA
	TPS6105X_FLASH_CURRENT_1,				//!< 200mA
	TPS6105X_FLASH_CURRENT_2,				//!< 300mA
	TPS6105X_FLASH_CURRENT_3,				//!< 400mA
	TPS6105X_FLASH_CURRENT_4,				//!< 500mA
	TPS6105X_FLASH_CURRENT_5,				//!< 700mA
	TPS6105X_FLASH_CURRENT_6,				//!< 900mA
	TPS6105X_FLASH_CURRENT_7,				//!< 1200mA
	TPS6105X_FLASH_CURRENT_NUM,
} TPS6105X_FLASH_CURRENT_t;

#define TPS6105X_FLASH_CURRENT__0			"150mA"
#define TPS6105X_FLASH_CURRENT__1			"200mA"
#define TPS6105X_FLASH_CURRENT__2			"300mA"
#define TPS6105X_FLASH_CURRENT__3			"400mA"
#define TPS6105X_FLASH_CURRENT__4			"500mA"
#define TPS6105X_FLASH_CURRENT__5			"700mA"
#define TPS6105X_FLASH_CURRENT__6			"900mA"
#define TPS6105X_FLASH_CURRENT__7			"1200mA"

/*!
 * \brief Possible modes
 */
typedef enum {
	TPS6105X_MODE_0,						//!< shutdown
	TPS6105X_MODE_1,						//!< torch
	TPS6105X_MODE_2,						//!< torch/flash
	TPS6105X_MODE_NUM,
} TPS6105X_MODE_t;

#define TPS6105X_MODE__0			"shutdown"
#define TPS6105X_MODE__1			"torch"
#define TPS6105X_MODE__2			"torch/flash"

int	TPS6105X_avin(int value);
TPS6105X_MODE_t	TPS6105X_mode(TPS6105X_MODE_t value);
TPS6105X_FLASH_CURRENT_t TPS6105X_flash_current(TPS6105X_FLASH_CURRENT_t value);
TPS6105X_TORCH_CURRENT_t TPS6105X_torch_current(TPS6105X_TORCH_CURRENT_t value);

#endif /* TPS6105X_H_ */
