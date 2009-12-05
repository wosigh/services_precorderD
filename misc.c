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
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "misc.h"

/*!
 * \brief A method to make sure a particular directory exists.
 *
 *
 * \param path The path that needs to exist
 *
 * \return true if directory exists, false if directory does not exist
 */
bool make_sure_dir_exists(char *path) {

	bool ret = false;

	DIR *dir;
	dir = opendir(path);
	if (dir) {
		ret = true;
		closedir(dir);
	} else if (mkdir(path, 0777)==0) {
		ret = true;
	}

	return ret;

}
