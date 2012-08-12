/*
 *  Copyright (C) 2012  Alexandre Courbot
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __CORE_COMPRESS_H
#define __CORE_COMPRESS_H

#include <stdint.h>
#include <zlib.h>

namespace Tagaini
{

template <class T>
int Compress(const T &src, T &dst);

template <class T>
int Decompress(const T &src, T &dst);

template <class T>
int Compress(const T &src, T &dst)
{
	static const size_t BUFSIZE = 1024;
	uint8_t temp_buffer[BUFSIZE];

	z_stream strm;
	strm.zalloc = 0;
	strm.zfree = 0;
	strm.next_in = (Bytef *)src.data();
	strm.avail_in = src.size();
	strm.next_out = temp_buffer;
	strm.avail_out = BUFSIZE;

	deflateInit(&strm, Z_BEST_COMPRESSION);

	while (strm.avail_in != 0)
	{
		int err = deflate(&strm, Z_NO_FLUSH);
		if (err != Z_OK)
			return err;
		if (strm.avail_out == 0)
		{
			dst.insert(dst.end(), temp_buffer, temp_buffer + BUFSIZE);
			strm.next_out = temp_buffer;
			strm.avail_out = BUFSIZE;
		}
	}

	int err = Z_OK;
	while (err == Z_OK)
	{
		if (strm.avail_out == 0)
		{
			dst.insert(dst.end(), temp_buffer, temp_buffer + BUFSIZE);
			strm.next_out = temp_buffer;
			strm.avail_out = BUFSIZE;
		}
		err = deflate(&strm, Z_FINISH);
	}

	if (!err == Z_STREAM_END)
		return err;
	dst.insert(dst.end(), temp_buffer, temp_buffer + BUFSIZE - strm.avail_out);
	deflateEnd(&strm);

	return 0;
}

template <class T>
int Decompress(const T &src, T &dst)
{
	static const size_t BUFSIZE = 1024;
	uint8_t temp_buffer[BUFSIZE];

	z_stream strm;
	strm.zalloc = 0;
	strm.zfree = 0;
	strm.next_in = (Bytef *)src.data();
	strm.avail_in = src.size();
	strm.next_out = temp_buffer;
	strm.avail_out = BUFSIZE;

	inflateInit(&strm);

	while (strm.avail_in != 0)
	{
		int err = inflate(&strm, Z_NO_FLUSH);
		if (err != Z_OK && err != Z_STREAM_END)
			return err;

		if (strm.avail_out == 0 || err == Z_STREAM_END)
		{
			dst.insert(dst.end(), temp_buffer, temp_buffer + BUFSIZE - strm.avail_out);
			strm.next_out = temp_buffer;
			strm.avail_out = BUFSIZE;
		}
		if (err == Z_STREAM_END) {
			return 0;
		}
	}

	int err = Z_OK;
	while (err == Z_OK)
	{
		if (strm.avail_out == 0)
		{
			dst.insert(dst.end(), temp_buffer, temp_buffer + BUFSIZE);
			strm.next_out = temp_buffer;
			strm.avail_out = BUFSIZE;
		}
		err = inflate(&strm, Z_FINISH);
	}

	if (!err == Z_STREAM_END)
		return err;

	dst.insert(dst.end(), temp_buffer, temp_buffer + BUFSIZE - strm.avail_out);
	inflateEnd(&strm);

	return 0;
}
}

#endif
