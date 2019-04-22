#pragma once
#include <cmath>
#include <iostream>
#define M_PI 3.1415926
class Vector2D {
	private:
		float x;
		float y;

	public:
		Vector2D() {
			this->x = 0;
			this->y = 0;
		}

		Vector2D(float x, float y) {
			this->x = x;
			this->y = y;
		}

		float get_x() {
			return x;
		};

		float get_y() {
			return y;
		};

		void set_y(float y) { y = y; };

		void set_x(float x) { x = x; };

		inline Vector2D& operator = (const Vector2D& rkVector)
		{
			x = rkVector.x;
			y = rkVector.y;

			return *this;
		}

		inline Vector2D& operator = (const float fScalar)
		{
			x = fScalar;
			y = fScalar;

			return *this;
		}

		inline Vector2D& operator-( const Vector2D& rkVector ) const{
			return Vector2D(x - rkVector.x, y - rkVector.y );
		}

		inline Vector2D& operator + (const Vector2D& rkVector) const {
			return Vector2D(x + rkVector.x, y + rkVector.y);
		}

		inline float length() const
		{
			return sqrt(x * x + y * y);
		}

		inline float distance(const Vector2D& rhs) const
		{
			return (*this - rhs).length();
		}

		inline float dotProduct(const Vector2D& vec) const
		{
			return x * vec.x + y * vec.y;
		}

		inline float crossProduct(const Vector2D& rkVector) const
		{
			return x * rkVector.y - y * rkVector.x;
		}

		inline float angleBetween(const Vector2D& other) const
		{
			float lenProduct = length() * other.length();
			// Divide by zero check
			if (lenProduct < 1e-6f) {
				lenProduct = 1e-6f;
			}
			float f = dotProduct(other) / lenProduct;

			if (f < -1.0) {
				f = -1.0;
			}
			else if (f > 1.0) {
				f = 1.0;
			}
			return acos(f);
		}

		inline float angleTo(const Vector2D& other) const
		{
			float angle = angleBetween(other);

			if (crossProduct(other) < 0)
				angle = (float)(2*M_PI) - angle;

			return angle;
		}

		inline float normalize()
		{
			float fLength = sqrt(x * x + y * y);

			if (fLength > (float)0.0f)
			{
				float fInvLength = 1.0f / fLength;
				x *= fInvLength;
				y *= fInvLength;
			}

			return fLength;
		}

};