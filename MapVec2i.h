//-----------------------------------------------------------------------------
// Copyright (c) 2017 Ricardo David CM (http://ricardo-david.com),
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef PATH_FIND_MAP_VEC_2I_H
#define PATH_FIND_MAP_VEC_2I_H

namespace PathFinding
{
	struct MapVec2i
	{
		int x;
		int y;
		
		inline const MapVec2i MapVec2i::operator+(const MapVec2i &other) const
		{
			return{ x + other.x, y + other.y };
		}

		inline const MapVec2i MapVec2i::operator-(const MapVec2i &other) const
		{
			return{ x - other.x, y - other.y };
		}

		inline const MapVec2i MapVec2i::operator*(int other) const
		{
			return{ x * other, y * other };
		}

		inline const bool MapVec2i::operator==(const MapVec2i & other) const
		{
			return x == other.x && y == other.y;
		}

		inline const bool MapVec2i::operator!=(const MapVec2i & other) const
		{
			return x != other.x || y != other.y;
		}
	};

	typedef MapVec2i MapCoord2i;

}

#endif // PATH_FIND_MAP_VEC_2I_H