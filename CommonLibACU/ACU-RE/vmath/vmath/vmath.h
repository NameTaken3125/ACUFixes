/* -*- C++ -*- */
/** @page license License
 * vmath, set of classes for computer graphics mathematics.
 * Copyright (c) 2005-2018, Jan Bartipan < barzto at gmail dot com >
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 *
 * - Redistributions of source code must retain the above copyright 
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright 
 *   notice, this list of conditions and the following disclaimer in 
 *   the documentation and/or other materials provided with the 
 *   distribution.
 * - Neither the names of its contributors may be used to endorse or 
 *   promote products derived from this software without specific 
 *   prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @mainpage Intro
 * Vector mathematics for computer graphics
 *
 * @section Features
 * <ul>
 *    <li> basic arithmetic operations - using operators </li>
 *    <li> basic linear algebra operations - such as transpose, dot product, etc. </li>
 *    <li> aliases for vertex coordinates - it means:
 *    <pre>
 *  Vector3f v;
 *  // use vertex coordinates
 *  v.x = 1; v.y = 2; v.z = -1;
 *
 *  // use texture coordinates
 *  v.s = 0; v.t = 1; v.u = 0.5;
 *  // use color coordinates
 *  v.r = 1; v.g = 0.5; v.b = 0;
 *    </pre>
 *    </li>
 *    <li> conversion constructor and assign operators - so you can assign a value of Vector3&lt;T1&gt; type 
 *    to a variable of Vector3&lt;T2&gt; type for any convertible T1, T2 type pairs. In other words, you can do this:
 *    <pre>
 *
 *  Vector3f f3; Vector3d d3 = f3;
 *  ...
 *  f3 = d3;
 *    </pre>
 *    </li>
 * </ul>
 *
 * @section Predefined types
 * <ul>
 * 	<li>Vector2 Two dimensional vector
 * 		<ul>
 * 			<li>float &mdash; Vector2f</li>
 * 			<li>double &mdash; Vector2d</li>
 * 			<li>int &mdash; Vector2i</li>
 * 		</ul>
 * 	</li>
 * 	<li>Vector3 Three dimensional vector
 * 		<ul>
 * 			<li>float &mdash; Vector3f</li>
 * 			<li>double &mdash; Vector3d</li>
 * 			<li>int &mdash; Vector3i</li>
 * 		</ul>
 * 	</li>
 * 	<li>Vector4 Four dimensional vector
 * 		<ul>
 * 			<li>float &mdash; Vector4f</li>
 * 			<li>double &mdash; Vector4d</li>
 * 			<li>int &mdash; Vector4i</li>
 * 		</ul>
 * 	</li>
 * 	<li>Matrix3 Matrix 3x3
 * 		<ul>
 * 			<li>float &mdash; Matrix3f</li>
 * 			<li>double &mdash; Matrix3d</li>
 * 			<li>int &mdash; Matrix3i</li>
 * 		</ul>
 * 	</li>
 * 	<li>Matrix4 Matrix 4x4
 * 		<ul>
 * 			<li>float &mdash; Matrix4f</li>
 * 			<li>double &mdash; Matrix4d</li>
 * 			<li>int &mdash; Matrix4i</li>
 * 		</ul>
 * 	</li>
 * 	<li> Quaternion
 * 		<ul>
 * 			<li>float &mdash; Quatf</li>
 * 			<li>double &mdash; Quatd</li>
 * 		</ul>
 * 		</li>
 * 	</li>
 * 	<li> Aabb3 axes-aligned bounding-box
 * 		<ul>
 * 			<li>float &mdash; Aabb3f</li>
 * 			<li>double &mdash; Aabb3d</li>
 * 		</ul>
 * 	</li>
 * </ul>
 */

// Modified 2011-06-12, Davide Bacchet (davide.bacchet at gmail dot com)
//                      added () operators with standard matrix notation (opposite wrt at() ).

#pragma warning( disable : 4244 ) 

#ifndef __vmath_Header_File__
#define __vmath_Header_File__

#include <cmath>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <cassert>

#ifdef VMATH_NAMESPACE
namespace VMATH_NAMESPACE
{
#endif

#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

#define DEG2RAD(x) ((x * M_PI) / 180.0)
//#define EPSILON (4.37114e-07)

const double epsilon = 4.37114e-05;
#define EPSILON epsilon

/**
 * Class for two dimensional vector.
 * There are three ways of accessing vector components.
 * Let's have <code>Vector2f v</code>, you can either:
 * <ul>
 * 	<li>access as position(x,y) &mdash; <code>v.x = v.y = 3;</code></li>
 * 	<li>access as texture coordinate (s,t) &mdash; <code>v.s = v.t = 3;</code></li>
 * 	<li>access via operator[] &mdash; <code>v[0] = v[1] = 3;</code></li>
 * </ul>
 */
template<class T>
class Vector2
{
public:
	union
	{
		/**
		 * First element of vector, alias for X-coordinate.
		 */
		T x;

		/**
		 * First element of vector, alias for S-coordinate.
		 * For textures notation.
		 */
		T s;
	};

	union
	{
		/**
		 * Second element of vector, alias for Y-coordinate.
		 */
		T y;

		/**
		 * Second element of vector, alias for T-coordinate.
		 * For textures notation.
		 */
		T t;
	};

	//----------------[ constructors ]--------------------------
	/**
	 * Creates and sets to (0,0)
	 */
	Vector2()
			: x(0), y(0)
	{
	}

	/**
	 * Creates and sets to (x,y)
	 * @param nx initial x-coordinate value
	 * @param ny initial y-coordinate value
	 */
	Vector2(T nx, T ny)
			: x(nx), y(ny)
	{
	}

	/**
	 * Copy constructor.
	 * @param src Source of data for new created instance.
	 */
	Vector2(const Vector2<T>& src)
			: x(src.x), y(src.y)
	{
	}

	/**
	 * Copy casting constructor.
	 * @param src Source of data for new created instance.
	 */
	template<class FromT>
	Vector2(const Vector2<FromT>& src)
			: x(static_cast<T>(src.x)), y(static_cast<T>(src.y))
	{
	}

	//----------------[ access operators ]-------------------
	/**
	 * Copy casting operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	template<class FromT>
	Vector2<T>& operator=(const Vector2<FromT>& rhs)
	{
		x = static_cast<T>(rhs.x);
		y = static_cast<T>(rhs.y);
		return *this;
	}

	/**
	 * Copy operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T>& operator=(const Vector2<T>& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}

	/**
	 * Array access operator
	 * @param n Array index
	 * @return For n = 0, reference to x coordinate, else reference to y 
	 * y coordinate.
	 */
	T& operator[](int n)
	{
		assert(n >= 0 && n <= 1);
		if (0 == n)
			return x;
		else
			return y;
	}

	/**
	 * Constant array access operator
	 * @param n Array index
	 * @return For n = 0, reference to x coordinate, else reference to y
	 * y coordinate.
	 */
	const T& operator[](int n) const
	{
		assert(n >= 0 && n <= 1);
		if (0 == n)
			return x;
		else
			return y;
	}

	//---------------[ vector aritmetic operator ]--------------
	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T> operator+(const Vector2<T>& rhs) const
	{
		return Vector2<T>(x + rhs.x, y + rhs.y);
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T> operator-(const Vector2<T>& rhs) const
	{
		return Vector2<T>(x - rhs.x, y - rhs.y);
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T> operator*(const Vector2<T>& rhs) const
	{
		return Vector2<T>(x * rhs.x, y * rhs.y);
	}

	/**
	 * Division operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T> operator/(const Vector2<T>& rhs) const
	{
		return Vector2<T>(x / rhs.x, y / rhs.y);
	}

	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T>& operator+=(const Vector2<T>& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	/**
	 * Substraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T>& operator-=(const Vector2<T>& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T>& operator*=(const Vector2<T>& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		return *this;
	}

	/**
	 * Division operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T>& operator/=(const Vector2<T>& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
		return *this;
	}

	//--------------[ scalar vector operator ]--------------------
	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T> operator+(T rhs) const
	{
		return Vector2<T>(x + rhs, y + rhs);
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T> operator-(T rhs) const
	{
		return Vector2<T>(x - rhs, y - rhs);
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T> operator*(T rhs) const
	{
		return Vector2<T>(x * rhs, y * rhs);
	}

	/**
	 * Division operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T> operator/(T rhs) const
	{
		return Vector2<T>(x / rhs, y / rhs);
	}

	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T>& operator+=(T rhs)
	{
		x += rhs;
		y += rhs;
		return *this;
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T>& operator-=(T rhs)
	{
		x -= rhs;
		y -= rhs;
		return *this;
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T>& operator*=(T rhs)
	{
		x *= rhs;
		y *= rhs;
		return *this;
	}

	/**
	 * Division operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector2<T>& operator/=(T rhs)
	{
		x /= rhs;
		y /= rhs;
		return *this;
	}

	//--------------[ equality operator ]------------------------
	/**
	 * Equality test operator
	 * @param rhs Right hand side argument of binary operator.
	 * @note Test of equality is based of threshold EPSILON value. To be two
	 * values equal, must satisfy this condition | lhs.x - rhs.y | < EPSILON,
	 * same for y-coordinate.
	 */
	bool operator==(const Vector2<T>& rhs) const
	{
		return (std::abs(x - rhs.x) < EPSILON) && (std::abs(y - rhs.y) < EPSILON);
	}

	/**
	 * Inequality test operator
	 * @param rhs Right hand side argument of binary operator.
	 * @return not (lhs == rhs) :-P
	 */
	bool operator!=(const Vector2<T>& rhs) const
	{
		return !(*this == rhs);
	}

	//-------------[ unary operations ]--------------------------
	/**
	 * Unary negate operator
	 * @return negated vector
	 */
	Vector2<T> operator-() const
	{
		return Vector2<T>(-x, -y);
	}

	//-------------[ size operations ]---------------------------
	/**
	 * Get length of vector.
	 * @return lenght of vector
	 */
	T length() const
	{
		return (T) std::sqrt(x * x + y * y);
	}

	/**
	 * Normalize vector
	 */
	void normalize()
	{
		T s = length();
		x /= s;
		y /= s;
	}

	/**
	 * Return square of length.
	 * @return length ^ 2
	 * @note This method is faster then length(). For comparison
	 * of length of two vector can be used just this value, instead
	 * of more expensive length() method.
	 */
	T lengthSq() const
	{
		return x * x + y * y;
	}

	//--------------[ misc. operations ]-----------------------
	/**
	 * Linear interpolation of two vectors
	 * @param fact Factor of interpolation. For translation from position
	 * of this vector to vector r, values of factor goes from 0.0 to 1.0.
	 * @param r Second Vector for interpolation 
	 * @note However values of fact parameter are reasonable only in interval
	 * [0.0 , 1.0], you can pass also values outside of this interval and you
	 * can get result (extrapolation?)
	 */
	Vector2<T> lerp(T fact, const Vector2<T>& r) const
	{
		return (*this) + (r - (*this)) * fact;
	}

	//-------------[ conversion ]-----------------------------
	/**
	 * Conversion to pointer operator
	 * @return Pointer to internally stored (in management of class Vector2<T>)
	 * used for passing Vector2<T> values to gl*2[fd] functions.
	 */
	operator T*()
	{
		return (T*) this;
	}
	/**
	 * Conversion to pointer operator
	 * @return Constant Pointer to internally stored (in management of class Vector2<T>)
	 * used for passing Vector2<T> values to gl*2[fd] functions.
	 */
	operator const T*() const
	{
		return (const T*) this;
	}

	//-------------[ output operator ]------------------------
	/**
	 * Output to stream operator
	 * @param lhs Left hand side argument of operator (commonly ostream instance). 
	 * @param rhs Right hand side argument of operator.
	 * @return Left hand side argument - the ostream object passed to operator.
	 */
	friend std::ostream& operator<<(std::ostream& lhs, const Vector2<T>& rhs)
	{
		lhs << "[" << rhs.x << "," << rhs.y << "]";
		return lhs;
	}

	/**
	 * Gets string representation.
	 */
	std::string toString() const
	{
		std::ostringstream oss;
		oss << *this;
		return oss.str();
	}
};

//--------------------------------------
// Typedef shortcuts for 2D vector
//-------------------------------------
/// Two dimensional Vector of floats
typedef class Vector2<float> Vector2f;
/// Two dimensional Vector of doubles
typedef class Vector2<double> Vector2d;
/// Two dimensional Vector of ints
typedef class Vector2<int> Vector2i;


template<class T> class Vector4;
template<class T> class Matrix4;

/**
 * Class for three dimensional vector.
 * There are four ways of accessing vector components.
 * Let's have <code>Vector3f v</code>, you can either:
 * <ul>
 * 	<li>access as position (x,y,z) &mdash; <code>v.x = v.y = v.z = 1;</code></li>
 * 	<li>access as texture coordinate (s,t,u) &mdash; <code>v.s = v.t = v.u = 1;</code></li>
 * 	<li>access as color (r,g,b) &mdash; <code>v.r = v.g = v.b = 1;</code></li>
 * 	<li>access via operator[] &mdash; <code>v[0] = v[1] = v[2] = 1;</code></li>
 * </ul>
 */
template<class T>
class Vector3
{
public:
	//T x, y, z;
	union
	{
		/**
		 * First element of vector, alias for X-coordinate.
		 */
		T x;

		/**
		 * First element of vector, alias for S-coordinate.
		 * For textures notation.
		 */
		T s;

		/**
		 * First element of vector, alias for R-coordinate.
		 * For color notation.
		 */
		T r;
	};

	union
	{
		/**
		 * Second element of vector, alias for Y-coordinate.
		 */
		T y;
		/**
		 * Second element of vector, alias for T-coordinate.
		 * For textures notation.
		 */
		T t;
		/**
		 * Second element of vector, alias for G-coordinate.
		 * For color notation.
		 */
		T g;
	};

	union
	{
		/**
		 * Third element of vector, alias for Z-coordinate.
		 */
		T z;

		/**
		 * Third element of vector, alias for U-coordinate.
		 * For textures notation.
		 */
		T u;
		/**
		 * Third element of vector, alias for B-coordinate.
		 * For color notation.
		 */
		T b;
	};

	//----------------[ constructors ]--------------------------
	/**
	 * Creates and sets to (0,0,0)
	 */
	Vector3()
			: x(0), y(0), z(0)
	{
	}

	/**
	 * Creates and sets to (x,y,z)
	 * @param nx initial x-coordinate value
	 * @param ny initial y-coordinate value
	 * @param nz initial z-coordinate value
	 */
	Vector3(T nx, T ny, T nz)
			: x(nx), y(ny), z(nz)
	{
	}

	/**
	 * Copy constructor.
	 * @param src Source of data for new created Vector3 instance.
	 */
	Vector3(const Vector3<T>& src)
			: x(src.x), y(src.y), z(src.z)
	{
	}

	/**
	 * Copy casting constructor.
	 * @param src Source of data for new created Vector3 instance.
	 */
	template<class FromT>
	Vector3(const Vector3<FromT>& src)
			: x(static_cast<T>(src.x)), y(static_cast<T>(src.y)), z(static_cast<T>(src.z))
	{
	}

	//----------------[ access operators ]-------------------
	/**
	 * Copy operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T> operator=(const Vector3<T>& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}

	/**
	 * Copy casting operator.
	 * @param rhs Right hand side argument of binary operator.
	 */
	template<class FromT>
	Vector3<T> operator=(const Vector3<FromT>& rhs)
	{
		x = static_cast<T>(rhs.x);
		y = static_cast<T>(rhs.y);
		z = static_cast<T>(rhs.z);
		return *this;
	}

	/**
	 * Array access operator
	 * @param n Array index
	 * @return For n = 0, reference to x coordinate, n = 1
	 * reference to y, else reference to z 
	 * y coordinate.
	 */
	T & operator[](int n)
	{
		assert(n >= 0 && n <= 2);
		if (0 == n)
			return x;
		else if (1 == n)
			return y;
		else
			return z;
	}

	/**
	 * Constant array access operator
	 * @param n Array index
	 * @return For n = 0, reference to x coordinate, n = 1
	 * reference to y, else reference to z
	 * y coordinate.
	 */
	const T & operator[](int n) const
	{
		assert(n >= 0 && n <= 2);
		if (0 == n)
			return x;
		else if (1 == n)
			return y;
		else
			return z;
	}

	//---------------[ vector arithmetic operator ]--------------
	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T> operator+(const Vector3<T>& rhs) const
	{
		return Vector3<T>(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T> operator-(const Vector3<T>& rhs) const
	{
		return Vector3<T>(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T> operator*(const Vector3<T>& rhs) const
	{
		return Vector3<T>(x * rhs.x, y * rhs.y, z * rhs.z);
	}

	/**
	 * Division operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T> operator/(const Vector3<T>& rhs) const
	{
		return Vector3<T>(x / rhs.x, y / rhs.y, z / rhs.z);
	}

	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T>& operator+=(const Vector3<T>& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T>& operator-=(const Vector3<T>& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T>& operator*=(const Vector3<T>& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}

	/**
	 * Division operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T>& operator/=(const Vector3<T>& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		return *this;
	}

	/**
	 * Dot product of two vectors.
	 * @param rhs Right hand side argument of binary operator.
	 */
	T dotProduct(const Vector3<T>& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	/**
	 * Cross product operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T> crossProduct(const Vector3<T>& rhs) const
	{
		return Vector3<T>(y * rhs.z - rhs.y * z, z * rhs.x - rhs.z * x, x * rhs.y - rhs.x * y);
	}

	//--------------[ scalar vector operator ]--------------------
	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T> operator+(T rhs) const
	{
		return Vector3<T>(x + rhs, y + rhs, z + rhs);
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T> operator-(T rhs) const
	{
		return Vector3<T>(x - rhs, y - rhs, z - rhs);
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T> operator*(T rhs) const
	{
		return Vector3<T>(x * rhs, y * rhs, z * rhs);
	}

	/**
	 * Division operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T> operator/(T rhs) const
	{
		return Vector3<T>(x / rhs, y / rhs, z / rhs);
	}

	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T>& operator+=(T rhs)
	{
		x += rhs;
		y += rhs;
		z += rhs;
		return *this;
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T>& operator-=(T rhs)
	{
		x -= rhs;
		y -= rhs;
		z -= rhs;
		return *this;
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T>& operator*=(T rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}

	/**
	 * Division operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T>& operator/=(T rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}

	//--------------[ Equality operator ]------------------------
	/**
	 * Equality test operator
	 * @param rhs Right hand side argument of binary operator.
	 * @note Test of equality is based of threshold EPSILON value. To be two
	 * values equal, must satisfy this condition | lhs.x - rhs.y | < EPSILON,
	 * same for y-coordinate, and z-coordinate.
	 */
	bool operator==(const Vector3<T>& rhs) const
	{
		return std::fabs(x - rhs.x) < EPSILON && std::fabs(y - rhs.y) < EPSILON && std::fabs(z - rhs.z) < EPSILON;
	}

	/**
	 * Inequality test operator
	 * @param rhs Right hand side argument of binary operator.
	 * @return not (lhs == rhs) :-P
	 */
	bool operator!=(const Vector3<T>& rhs) const
	{
		return !(*this == rhs);
	}

	//-------------[ unary operations ]--------------------------
	/**
	 * Unary negate operator
	 * @return negated vector
	 */
	Vector3<T> operator-() const
	{
		return Vector3<T>(-x, -y, -z);
	}

	//-------------[ size operations ]---------------------------
	/**
	 * Get length of vector.
	 * @return lenght of vector
	 */
	T length() const
	{
		return (T) std::sqrt(x * x + y * y + z * z);
	}

	/**
	 * Return square of length.
	 * @return length ^ 2
	 * @note This method is faster then length(). For comparison
	 * of length of two vector can be used just this value, instead
	 * of more expensive length() method.
	 */
	T lengthSq() const
	{
		return x * x + y * y + z * z;
	}

	/**
	 * Normalize vector
	 */
	void normalize()
	{
		T s = length();
		x /= s;
		y /= s;
		z /= s;
	}

	/**
	 * Make new normalized vector
	 */
	Vector3<T> normalized()
	{
		Vector3<T> retval = *this;
		retval.normalize();
		return retval;
	}

	//------------[ other operations ]---------------------------
	/**
	 * Rotate vector around three axis.
	 * @param ax Angle (in degrees) to be rotated around X-axis.
	 * @param ay Angle (in degrees) to be rotated around Y-axis.
	 * @param az Angle (in degrees) to be rotated around Z-axis.
	 */
	void rotate(T ax, T ay, T az)
	{
		T a = cos(DEG2RAD(ax));
		T b = sin(DEG2RAD(ax));
		T c = cos(DEG2RAD(ay));
		T d = sin(DEG2RAD(ay));
		T e = cos(DEG2RAD(az));
		T f = sin(DEG2RAD(az));
		T nx = c * e * x - c * f * y + d * z;
		T ny = (a * f + b * d * e) * x + (a * e - b * d * f) * y - b * c * z;
		T nz = (b * f - a * d * e) * x + (a * d * f + b * e) * y + a * c * z;
		x = nx;
		y = ny;
		z = nz;

	}

	Vector3<T> transformed(const Matrix4<T>& mat) const
	{
		return (mat * Vector4<T>(x, y, z, 1)).xyz();
	}

	/**
	 * Linear interpolation of two vectors
	 * @param fact Factor of interpolation. For translation from positon
	 * of this vector to vector r, values of factor goes from 0.0 to 1.0.
	 * @param r Second Vector for interpolation 
	 * @note However values of fact parameter are reasonable only in interval
	 * [0.0 , 1.0], you can pass also values outside of this interval and you
	 * can get result (extrapolation?)
	 */
	Vector3<T> lerp(T fact, const Vector3<T>& r) const
	{
		return (*this) + (r - (*this)) * fact;
	}

	//-------------[ conversion ]-----------------------------

	/**
	 * Conversion to pointer operator
	 * @return Pointer to internally stored (in management of class Vector3<T>)
	 * used for passing Vector3<T> values to gl*3[fd] functions.
	 */
	operator T*()
	{
		return (T*) this;
	}

	/**
	 * Conversion to pointer operator
	 * @return Constant Pointer to internally stored (in management of class Vector3<T>)
	 * used for passing Vector3<T> values to gl*3[fd] functions.
	 */
	operator const T*() const
	{
		return (const T*) this;
	}

	//-------------[ output operator ]------------------------
	/**
	 * Output to stream operator
	 * @param lhs Left hand side argument of operator (commonly ostream instance). 
	 * @param rhs Right hand side argument of operator.
	 * @return Left hand side argument - the ostream object passed to operator.
	 */
	friend std::ostream& operator<<(std::ostream& lhs, const Vector3<T> rhs)
	{
		lhs << "[" << rhs.x << "," << rhs.y << "," << rhs.z << "]";
		return lhs;
	}

	/**
	 * Gets string representation.
	 */
	std::string toString() const
	{
		std::ostringstream oss;
		oss << *this;
		return oss.str();
	}
};

/// Three dimensional Vector of floats
typedef Vector3<float> Vector3f;
/// Three dimensional Vector of doubles
typedef Vector3<double> Vector3d;
/// Three dimensional Vector of ints
typedef Vector3<int> Vector3i;

/**
 * Class for four dimensional vector.
  * There are four ways of accessing vector components.
 * Let's have <code>Vector4f v</code>, you can either:
 * <ul>
 * 	<li>access as position in projective space (x,y,z,w) &mdash; <code>v.x = v.y = v.z = v.w = 1;</code></li>
 * 	<li>access as texture coordinate (s,t,u,v) &mdash; <code>v.s = v.t = v.u = v.v = 1;</code></li>
 * 	<li>access as color (r,g,b,a) &mdash; <code>v.r = v.g = v.b = v.a = 1;</code></li>
 * 	<li>access via operator[] &mdash; <code>v[0] = v[1] = v[2] = v[3] = 1;</code></li>
 * </ul>
 */
template<class T>
class Vector4
{
public:

	union
	{
		/**
		 * First element of vector, alias for R-coordinate.
		 * For color notation.
		 */
		T r
		/**
		 * First element of vector, alias for X-coordinate.
		 */;
		T x;
	};

	union
	{
		/**
		 * Second element of vector, alias for G-coordinate.
		 * For color notation.
		 */
		T g;
		/**
		 * Second element of vector, alias for Y-coordinate.
		 */
		T y;
	};

	union
	{
		/**
		 * Third element of vector, alias for B-coordinate.
		 * For color notation.
		 */
		T b;
		/**
		 * Third element of vector, alias for Z-coordinate.
		 */
		T z;
	};

	union
	{
		/**
		 * Fourth element of vector, alias for A-coordinate.
		 * For color notation. This represnt aplha chanell
		 */
		T a;
		/**
		 * First element of vector, alias for W-coordinate.
		 * @note For vectors (such as normals) should be set to 0.0
		 * For vertices should be set to 1.0
		 */
		T w;
	};

	//----------------[ constructors ]--------------------------
	/**
	 * Creates and sets to (0,0,0,0)
	 */
	Vector4()
			: x(0), y(0), z(0), w(0)
	{
	}

	/**
	 * Creates and sets to (x,y,z,z)
	 * @param nx initial x-coordinate value (R)
	 * @param ny initial y-coordinate value (G)
	 * @param nz initial z-coordinate value (B)
	 * @param nw initial w-coordinate value (Alpha)
	 */
	Vector4(T nx, T ny, T nz, T nw)
			: x(nx), y(ny), z(nz), w(nw)
	{
	}

	/**
	 * Copy constructor.
	 * @param src Source of data for new created Vector4 instance.
	 */
	Vector4(const Vector4<T>& src)
			: x(src.x), y(src.y), z(src.z), w(src.w)
	{
	}

	/**
	 * Copy casting constructor.
	 * @param src Source of data for new created Vector4 instance.
	 */
	template<class FromT>
	Vector4(const Vector4<FromT>& src)
			: x(static_cast<T>(src.x)), y(static_cast<T>(src.y)), z(static_cast<T>(src.z)), w(static_cast<T>(src.w))
	{
	}

	Vector4(const Vector3<T>& src, T w)
		: x(src.x), y(src.y), z(src.z), w(w)
    {}

	template <typename FromT>
	Vector4(const Vector3<FromT>& src, FromT w)
		: x(static_cast<T>(src.x)), y(static_cast<T>(src.y)), z(static_cast<T>(src.z)), w(static_cast<T>(w))
    {}

	//----------------[ access operators ]-------------------
	/**
	 * Copy operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T> operator=(const Vector4<T>& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = rhs.w;
		return *this;
	}

	/**
	 * Copy casting operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	template<class FromT>
	Vector4<T> operator=(const Vector4<FromT>& rhs)
	{
		x = static_cast<T>(rhs.x);
		y = static_cast<T>(rhs.y);
		z = static_cast<T>(rhs.z);
		w = static_cast<T>(rhs.w);
		return *this;
	}

	/**
	 * Array access operator
	 * @param n Array index
	 * @return For n = 0, reference to x coordinate, n = 1
	 * reference to y coordinate, n = 2 reference to z,  
	 * else reference to w coordinate.
	 */
	T & operator[](int n)
	{
		assert(n >= 0 && n <= 3);
		if (0 == n)
			return x;
		else if (1 == n)
			return y;
		else if (2 == n)
			return z;
		else
			return w;
	}

	/**
	 * Array access operator
	 * @param n Array index
	 * @return For n = 0, reference to x coordinate, n = 1
	 * reference to y coordinate, n = 2 reference to z,
	 * else reference to w coordinate.
	 */
	const T & operator[](int n) const
	{
		assert(n >= 0 && n <= 3);
		if (0 == n)
			return x;
		else if (1 == n)
			return y;
		else if (2 == n)
			return z;
		else
			return w;
	}

	//---------------[ vector aritmetic operator ]--------------
	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T> operator+(const Vector4<T>& rhs) const
	{
		return Vector4<T>(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T> operator-(const Vector4<T>& rhs) const
	{
		return Vector4<T>(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T> operator*(const Vector4<T> rhs) const
	{
		return Vector4<T>(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
	}

	/**
	 * Division operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T> operator/(const Vector4<T>& rhs) const
	{
		return Vector4<T>(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
	}

	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T>& operator+=(const Vector4<T>& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;
		return *this;
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T>& operator-=(const Vector4<T>& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;
		return *this;
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T>& operator*=(const Vector4<T>& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		w *= rhs.w;
		return *this;
	}

	/**
	 * Division operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T>& operator/=(const Vector4<T>& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		w /= rhs.w;
		return *this;
	}

	//--------------[ equiality operator ]------------------------
	/**
	 * Equality test operator
	 * @param rhs Right hand side argument of binary operator.
	 * @note Test of equality is based of threshold EPSILON value. To be two
	 * values equal, must satisfy this condition | lhs.x - rhs.y | < EPSILON,
	 * same for y-coordinate, z-coordinate, and w-coordinate.
	 */
	bool operator==(const Vector4<T>& rhs) const
	{
		return std::fabs(x - rhs.x) < EPSILON && std::fabs(y - rhs.y) < EPSILON && std::fabs(z - rhs.z) < EPSILON
				&& std::fabs(w - rhs.w) < EPSILON;
	}

	/**
	 * Inequality test operator
	 * @param rhs Right hand side argument of binary operator.
	 * @return not (lhs == rhs) :-P
	 */
	bool operator!=(const Vector4<T>& rhs) const
	{
		return !(*this == rhs);
	}

	//-------------[ unary operations ]--------------------------
	/**
	 * Unary negate operator
	 * @return negated vector
	 */
	Vector4<T> operator-() const
	{
		return Vector4<T>(-x, -y, -z, -w);
	}

	//--------------[ scalar vector operator ]--------------------

	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T> operator+(T rhs) const
	{
		return Vector4<T>(x + rhs, y + rhs, z + rhs, w + rhs);
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T> operator-(T rhs) const
	{
		return Vector4<T>(x - rhs, y - rhs, z - rhs, w - rhs);
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T> operator*(T rhs) const
	{
		return Vector4<T>(x * rhs, y * rhs, z * rhs, w * rhs);
	}

	/**
	 * Division operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T> operator/(T rhs) const
	{
		return Vector4<T>(x / rhs, y / rhs, z / rhs, w / rhs);
	}

	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T>& operator+=(T rhs)
	{
		x += rhs;
		y += rhs;
		z += rhs;
		w += rhs;
		return *this;
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T>& operator-=(T rhs)
	{
		x -= rhs;
		y -= rhs;
		z -= rhs;
		w -= rhs;
		return *this;
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T>& operator*=(T rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		w *= rhs;
		return *this;
	}

	/**
	 * Division operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T>& operator/=(T rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		w /= rhs;
		return *this;
	}

	//-------------[ size operations ]---------------------------
	/**
	 * Get length of vector.
	 * @return lenght of vector
	 */
	T length() const
	{
		return (T) std::sqrt(x * x + y * y + z * z + w * w);
	}

	/**
	 * Normalize vector
	 */
	void normalize()
	{
		T s = length();
		x /= s;
		y /= s;
		z /= s;
		w /= s;
	}

	/**
	 * Return square of length.
	 * @return length ^ 2
	 * @note This method is faster then length(). For comparison
	 * of length of two vector can be used just this value, instead
	 * of more expensive length() method.
	 */
	T lengthSq() const
	{
		return x * x + y * y + z * z + w * w;
	}

	//--------------[ misc. operations ]-----------------------
	/**
	 * Linear interpolation of two vectors
	 * @param fact Factor of interpolation. For translation from position
	 * of this vector to vector r, values of factor goes from 0.0 to 1.0.
	 * @param r Second Vector for interpolation 
	 * @note However values of fact parameter are reasonable only in interval
	 * [0.0 , 1.0], you can pass also values outside of this interval and you
	 * can get result (extrapolation?)
	 */
	Vector4<T> lerp(T fact, const Vector4<T>& r) const
	{
		return (*this) + (r - (*this)) * fact;
	}

	//-------------[ conversion ]-----------------------------

	/**
	 * Conversion to pointer operator
	 * @return Pointer to internally stored (in management of class Vector4<T>)
	 * used for passing Vector4<T> values to gl*4[fd] functions.
	 */
	operator T*()
	{
		return (T*) this;
	}

	/**
	 * Conversion to pointer operator
	 * @return Constant Pointer to internally stored (in management of class Vector4<T>)
	 * used for passing Vector4<T> values to gl*4[fd] functions.
	 */
	operator const T*() const
	{
		return (const T*) this;
	}

	/**
	 * Gets 3D vector. Note that the output is divided by w coordinate to apply projection
	 * transform. If the w coordinate is equal to zero, the result is not divided.
	 * @return (x/w, y/w, z/w) iff w != 0 otherwise (x,y,z)
	 */
	Vector3<T> xyz() const
	{
		if (w == 0 || w == 1)
			return Vector3<T>(x,y,z);

		const T invW = 1.0 / w;
		return Vector3<T>(x * invW, y * invW, z * invW);
	}
    Vector2<T> xy()
    {
        return Vector2<T>(x, y);
    }

	//-------------[ output operator ]------------------------
	/**
	 * Output to stream operator
	 * @param lhs Left hand side argument of operator (commonly ostream instance). 
	 * @param rhs Right hand side argument of operator.
	 * @return Left hand side argument - the ostream object passed to operator.
	 */
	friend std::ostream& operator<<(std::ostream& lhs, const Vector4<T>& rhs)
	{
		lhs << "[" << rhs.x << "," << rhs.y << "," << rhs.z << "," << rhs.w << "]";
		return lhs;
	}

	/**
	 * Gets string representation.
	 */
	std::string toString() const
	{
		std::ostringstream oss;
		oss << *this;
		return oss.str();
	}

};

/// Three dimensional Vector of floats
typedef Vector4<float> Vector4f;
/// Three dimensional Vector of doubles
typedef Vector4<double> Vector4d;
/// Three dimensional Vector of ints
typedef Vector4<int> Vector4i;

/**
 * Class for matrix 3x3. 
 * @note Data stored in this matrix are in column major order. This arrangement suits OpenGL.
 * If you're using row major matrix, consider using fromRowMajorArray as way for construction
 * Matrix3<T> instance.
 */
template<class T>
class Matrix3
{
public:
	/// Data stored in column major order
	T data[9];

	//--------------------------[ constructors ]-------------------------------
	/**
	 * Creates identity matrix
	 */
	Matrix3()
	{
		for (int i = 0; i < 9; i++)
			data[i] = (i % 4) ? 0 : 1;
	}

	/**
	 * Copy matrix values from array (these data must be in column
	 * major order!)
	 */
	Matrix3(const T * dt)
	{
		std::memcpy(data, dt, sizeof(T) * 9);
	}

	/**
	 * Copy constructor.
	 * @param src Data source for new created instance of Matrix3
	 */
	Matrix3(const Matrix3<T>& src)
	{
		std::memcpy(data, src.data, sizeof(T) * 9);
	}

	/**
	 * Copy casting constructor.
	 * @param src Data source for new created instance of Matrix3
	 */
	template<class FromT>
	Matrix3(const Matrix3<FromT>& src)
	{
		for (int i = 0; i < 9; i++)
		{
			data[i] = static_cast<T>(src.data[i]);
		}
	}

	/**
	 * Resets matrix to be identity matrix
	 */
	void identity()
	{
		for (int i = 0; i < 9; i++)
			data[i] = (i % 4) ? 0 : 1;
	}

	/**
	 * Creates rotation matrix by rotation around axis.
	 * @param xDeg Angle (in degrees) of rotation around axis X.
	 * @param yDeg Angle (in degrees) of rotation around axis Y.
	 * @param zDeg Angle (in degrees) of rotation around axis Z.
	 */
	static Matrix3<T> createRotationAroundAxis(T xDeg, T yDeg, T zDeg)
	{
		T xRads(DEG2RAD(xDeg));
		T yRads(DEG2RAD(yDeg));
		T zRads(DEG2RAD(zDeg));

		Matrix3<T> ma, mb, mc;
		float ac = cos(xRads);
		float as = sin(xRads);
		float bc = cos(yRads);
		float bs = sin(yRads);
		float cc = cos(zRads);
		float cs = sin(zRads);

		ma.at(1, 1) = ac;
		ma.at(2, 1) = as;
		ma.at(1, 2) = -as;
		ma.at(2, 2) = ac;

		mb.at(0, 0) = bc;
		mb.at(2, 0) = -bs;
		mb.at(0, 2) = bs;
		mb.at(2, 2) = bc;

		mc.at(0, 0) = cc;
		mc.at(1, 0) = cs;
		mc.at(0, 1) = -cs;
		mc.at(1, 1) = cc;

		Matrix3<T> ret = ma * mb * mc;
		return ret;
	}

	/**
	 * Creates rotation matrix from ODE Matrix.
	 */
	template<class It>
	static Matrix3<T> fromOde(const It* mat)
	{
		Matrix3<T> ret;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				ret.at(i, j) = static_cast<T>(mat[j * 4 + i]);
			}
		}
		return ret;
	}

	/**
	 * Creates new matrix 3x3 from array that represents such matrix 3x3
	 * as array of tightly packed elements in row major order.
	 * @param arr An array of elements for 3x3 matrix in row major order.
	 * @return An instance of Matrix3<T> representing @a arr
	 */
	template<class FromT>
	static Matrix3<T> fromRowMajorArray(const FromT* arr)
	{
		const T retData[] =
		{ static_cast<T>(arr[0]), static_cast<T>(arr[3]), static_cast<T>(arr[6]), static_cast<T>(arr[1]),
				static_cast<T>(arr[4]), static_cast<T>(arr[7]), static_cast<T>(arr[2]), static_cast<T>(arr[5]),
				static_cast<T>(arr[8]) };

		return retData;
	}

	/**
	 * Creates new matrix 3x3 from array that represents such matrix 3x3
	 * as array of tightly packed elements in column major order.
	 * @param arr An array of elements for 3x3 matrix in column major order.
	 * @return An instance of Matrix3<T> representing @a arr
	 */
	template<class FromT>
	static Matrix3<T> fromColumnMajorArray(const FromT* arr)
	{
		const T retData[] =
		{ static_cast<T>(arr[0]), static_cast<T>(arr[1]), static_cast<T>(arr[2]), static_cast<T>(arr[3]),
				static_cast<T>(arr[4]), static_cast<T>(arr[5]), static_cast<T>(arr[6]), static_cast<T>(arr[7]),
				static_cast<T>(arr[8]) };

		return retData;
	}

	//---------------------[ equiality operators ]------------------------------
	/**
	 * Equality test operator
	 * @param rhs Right hand side argument of binary operator.
	 * @note Test of equality is based of threshold EPSILON value. To be two
	 * values equal, must satisfy this condition all elements of matrix 
	 * | lhs[i] - rhs[i] | < EPSILON,
	 * same for y-coordinate, z-coordinate, and w-coordinate.
	 */
	bool operator==(const Matrix3<T>& rhs) const
	{
		for (int i = 0; i < 9; i++)
		{
			if (std::fabs(data[i] - rhs.data[i]) >= EPSILON)
				return false;
		}
		return true;
	}

	/**
	 * Inequality test operator
	 * @param rhs Right hand side argument of binary operator.
	 * @return not (lhs == rhs) :-P
	 */
	bool operator!=(const Matrix3<T>& rhs) const
	{
		return !(*this == rhs);
	}

	//---------------------[ access operators ]---------------------------------
	/**
	 * Get reference to element at position (x,y).
	 * @param x Number of column (0..2)
	 * @param y Number of row (0..2)
	 */
	T& at(int x, int y)
	{
		assert(x >= 0 && x < 3);
		assert(y >= 0 && y < 3);
		return data[x * 3 + y];
	}

	/**
	 * Get constant reference to element at position (x,y).
	 * @param x Number of column (0..2)
	 * @param y Number of row (0..2)
	 */
	const T& at(int x, int y) const
	{
		assert(x >= 0 && x < 3);
		assert(y >= 0 && y < 3);
		return data[x * 3 + y];
	}

	/**
	 * Get reference to element at position (i,j), with math matrix notation
	 * @param i Number of row (1..3)
	 * @param j Number of column (1..3)
	 */
	T& operator()(int i, int j)
	{
		assert(i >= 1 && i <= 3);
		assert(j >= 1 && j <= 3);
		return data[(j - 1) * 3 + i - 1];
	}

	/**
	 * Get constant reference to element at position (i,j), with math matrix notation
	 * @param i Number of row (1..3)
	 * @param j Number of column (1..3)
	 */
	const T& operator()(int i, int j) const
	{
		assert(i >= 1 && i <= 3);
		assert(j >= 1 && j <= 3);
		return data[(j - 1) * 3 + i - 1];
	}

	/**
	 * Copy operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix3<T>& operator=(const Matrix3<T>& rhs)
	{
		std::memcpy(data, rhs.data, sizeof(T) * 9);
		return *this;
	}

	/**
	 * Copy casting operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	template<class FromT>
	Matrix3<T>& operator=(const Matrix3<FromT>& rhs)
	{
		for (int i = 0; i < 9; i++)
		{
			data[i] = static_cast<T>(rhs.data[i]);
		}
		return *this;
	}

	/**
	 * Copy operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix3<T>& operator=(const T* rhs)
	{
		std::memcpy(data, rhs, sizeof(T) * 9);
		return *this;
	}

	/*Matrix3<T> & operator=(const double* m)
	 {
	 for (int i = 0; i < 9; i++) data[i] = (T)m[i];
	 return * this;
	 }*/

	//--------------------[ matrix with matrix operations ]---------------------
	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix3<T> operator+(const Matrix3<T>& rhs) const
	{
		Matrix3<T> ret;
		for (int i = 0; i < 9; i++)
			ret.data[i] = data[i] + rhs.data[i];
		return ret;
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix3<T> operator-(const Matrix3<T>& rhs) const
	{
		Matrix3<T> ret;
		for (int i = 0; i < 9; i++)
			ret.data[i] = data[i] - rhs.data[i];
		return ret;
	}

	//--------------------[ matrix with scalar operations ]---------------------
	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix3<T> operator+(T rhs) const
	{
		Matrix3<T> ret;
		for (int i = 0; i < 9; i++)
			ret.data[i] = data[i] + rhs;
		return ret;
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix3<T> operator-(T rhs) const
	{
		Matrix3<T> ret;
		for (int i = 0; i < 9; i++)
			ret.data[i] = data[i] - rhs;
		return ret;
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix3<T> operator*(T rhs) const
	{
		Matrix3<T> ret;
		for (int i = 0; i < 9; i++)
			ret.data[i] = data[i] * rhs;
		return ret;
	}

	/**
	 * Division operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix3<T> operator/(T rhs) const
	{
		Matrix3<T> ret;
		for (int i = 0; i < 9; i++)
			ret.data[i] = data[i] / rhs;
		return ret;
	}

	//--------------------[ multiply operators ]--------------------------------
	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T> operator*(const Vector3<T>& rhs) const
	{
		return Vector3<T>(data[0] * rhs.x + data[3] * rhs.y + data[6] * rhs.z,
				data[1] * rhs.x + data[4] * rhs.y + data[7] * rhs.z,
				data[2] * rhs.x + data[5] * rhs.y + data[8] * rhs.z);
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix3<T> operator*(Matrix3<T> rhs) const
	{
		Matrix3<T> w;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				T n = 0;
				for (int k = 0; k < 3; k++)
					n += rhs.at(i, k) * at(k, j);
				w.at(i, j) = n;
			}
		}
		return w;

	}

	//---------------------------[ misc operations ]----------------------------
	/**
	 * Transpose matrix.
	 */
	Matrix3<T> transpose()
	{
		Matrix3<T> ret;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				ret.at(i, j) = at(j, i);
			}
		}
		return ret;
	}

	/**
	 * Linear interpolation of two matrices
	 * @param fact Factor of interpolation. For translation from positon
	 * of this matrix (lhs) to matrix rhs, values of factor goes from 0.0 to 1.0.
	 * @param rhs Second Matrix for interpolation 
	 * @note However values of fact parameter are reasonable only in interval
	 * [0.0 , 1.0], you can pass also values outside of this interval and you
	 * can get result (extrapolation?)
	 */
	Matrix3<T> lerp(T fact, const Matrix3<T>& rhs) const
	{
		Matrix3<T> ret = (*this) + (rhs - (*this)) * fact;
		return ret;
	}

	T det()
	{
		return +at(0, 0) * at(1, 1) * at(2, 2) + at(0, 1) * at(1, 2) * at(2, 0) + at(0, 2) * at(1, 0) * at(2, 1)
				- at(0, 0) * at(1, 2) * at(2, 1) - at(0, 1) * at(1, 0) * at(2, 2) - at(0, 2) * at(1, 1) * at(2, 0);
	}

	/**
	 * Computes inverse matrix
	 * @return Inverse matrix of this matrix.
	 */
	Matrix3<T> inverse()
	{
		Matrix3<T> ret;
		ret.at(0, 0) = at(1, 1) * at(2, 2) - at(2, 1) * at(1, 2);
		ret.at(0, 1) = at(2, 1) * at(0, 2) - at(0, 1) * at(2, 2);
		ret.at(0, 2) = at(0, 1) * at(1, 2) - at(1, 1) * at(0, 2);
		ret.at(1, 0) = at(2, 0) * at(1, 2) - at(1, 0) * at(2, 2);
		ret.at(1, 1) = at(0, 0) * at(2, 2) - at(2, 0) * at(0, 2);
		ret.at(1, 2) = at(1, 0) * at(0, 2) - at(0, 0) * at(1, 2);
		ret.at(2, 0) = at(1, 0) * at(2, 1) - at(2, 0) * at(1, 1);
		ret.at(2, 1) = at(2, 0) * at(0, 1) - at(0, 0) * at(2, 1);
		ret.at(2, 2) = at(0, 0) * at(1, 1) - at(1, 0) * at(0, 1);
		return ret * (1.0f / det());
	}

	//-------------[ conversion ]-----------------------------

	/**
	 * Conversion to pointer operator
	 * @return Pointer to internally stored (in management of class Matrix3<T>)
	 * used for passing Matrix3<T> values to gl*[fd]v functions.
	 */
	operator T*()
	{
		return (T*) data;
	}

	/**
	 * Conversion to pointer operator
	 * @return Constant Pointer to internally stored (in management of class Matrix3<T>)
	 * used for passing Matrix3<T> values to gl*[fd]v functions.
	 */
	operator const T*() const
	{
		return (const T*) data;
	}

	//----------[ output operator ]----------------------------
	/**
	 * Output to stream operator
	 * @param lhs Left hand side argument of operator (commonly ostream instance). 
	 * @param rhs Right hand side argument of operator.
	 * @return Left hand side argument - the ostream object passed to operator.
	 */
	friend std::ostream& operator <<(std::ostream& lhs, const Matrix3<T>& rhs)
	{
		for (int i = 0; i < 3; i++)
		{
			lhs << "|\t";
			for (int j = 0; j < 3; j++)
			{
				lhs << rhs.at(j, i) << "\t";
			}
			lhs << "|" << std::endl;
		}
		return lhs;
	}

	/**
	 * Gets string representation.
	 */
	std::string toString() const
	{
		std::ostringstream oss;
		oss << *this;
		return oss.str();
	}
};

/// Matrix 3x3 of floats
typedef Matrix3<float> Matrix3f;
/// Matrix 3x3 of doubles
typedef Matrix3<double> Matrix3d;
/// Matrix 3x3 of int
typedef Matrix3<int> Matrix3i;

/**
 * Class for matrix 4x4 
 * @note Data stored in this matrix are in column major order. This arrangement suits OpenGL.
 * If you're using row major matrix, consider using fromRowMajorArray as way for construction
 * Matrix4<T> instance.
 */
template<class T>
class Matrix4
{
public:
	/// Data stored in column major order
	T data[16];

	//--------------------------[ constructors ]-------------------------------
	/**
	 *Creates identity matrix
	 */
	Matrix4()
	{
		for (int i = 0; i < 16; i++)
			data[i] = (T)((i % 5) ? 0 : 1);
	}

	/**
	 * Copy matrix values from array (these data must be in column
	 * major order!)
	 */
	Matrix4(const T * dt)
	{
		std::memcpy(data, dt, sizeof(T) * 16);
	}

	/**
	 * Copy constructor.
	 * @param src Data source for new created instance of Matrix4.
	 */
	Matrix4(const Matrix4<T>& src)
	{
		std::memcpy(data, src.data, sizeof(T) * 16);
	}

	/**
	 * Copy casting constructor.
	 * @param src Data source for new created instance of Matrix4.
	 */
	template<class FromT>
	Matrix4(const Matrix4<FromT>& src)
	{
		for (int i = 0; i < 16; i++)
		{
			data[i] = static_cast<T>(src.data[i]);
		}
	}

	/**
	 * Resets matrix to be identity matrix
	 */
	void identity()
	{
		for (int i = 0; i < 16; i++)
			data[i] = (i % 5) ? 0 : 1;
	}

	/**
	 * Creates rotation matrix by rotation around axis.
	 * @param xDeg Angle (in degrees) of rotation around axis X.
	 * @param yDeg Angle (in degrees) of rotation around axis Y.
	 * @param zDeg Angle (in degrees) of rotation around axis Z.
	 */
	static Matrix4<T> createRotationAroundAxis(T xDeg, T yDeg, T zDeg)
	{
		T xRads(DEG2RAD(xDeg));
		T yRads(DEG2RAD(yDeg));
		T zRads(DEG2RAD(zDeg));

		Matrix4<T> ma, mb, mc;
		float ac = cos(xRads);
		float as = sin(xRads);
		float bc = cos(yRads);
		float bs = sin(yRads);
		float cc = cos(zRads);
		float cs = sin(zRads);

		ma.at(1, 1) = ac;
		ma.at(2, 1) = as;
		ma.at(1, 2) = -as;
		ma.at(2, 2) = ac;

		mb.at(0, 0) = bc;
		mb.at(2, 0) = -bs;
		mb.at(0, 2) = bs;
		mb.at(2, 2) = bc;

		mc.at(0, 0) = cc;
		mc.at(1, 0) = cs;
		mc.at(0, 1) = -cs;
		mc.at(1, 1) = cc;

		/*std::cout << "RotVec = " << a << "," << b << "," << c << std::endl;
		 std::cout << "Rx = " << std::endl << ma;
		 std::cout << "Ry = " << std::endl << mb;
		 std::cout << "Rz = " << std::endl << mc;*/

		Matrix4<T> ret = ma * mb * mc;
		//std::cout << "Result = " << std::endl << ma * (mb * mc);

		return ret;
	}

	/// Creates translation matrix
	/**
	 * Creates translation matrix.
	 * @param x X-direction translation
	 * @param y Y-direction translation
	 * @param z Z-direction translation
	 * @param w for W-coordinate translation (implicitly set to 1)
	 */
	static Matrix4<T> createTranslation(T x, T y, T z, T w = 1)
	{
		Matrix4 ret;
		ret.at(3, 0) = x;
		ret.at(3, 1) = y;
		ret.at(3, 2) = z;
		ret.at(3, 3) = w;

		return ret;
	}
    template<typename VecType>
	static Matrix4<T> createTranslation(const Vector3<VecType>& vec3)
	{
		Matrix4 ret;
		ret.at(3, 0) = vec3.x;
		ret.at(3, 1) = vec3.y;
		ret.at(3, 2) = vec3.z;
		ret.at(3, 3) = 1;

		return ret;
	}
	
	/**
	 * Create scale matrix with @a sx, @a sy, and @a sz
	 * being values of matrix main diagonal.
	 * @param sx Scale in X-axis
	 * @param sy Scale in Y-axis
	 * @param sz Scale in Z-axis
	 * @return Transform matrix 4x4 with scale transformation.
	 */
	static Matrix4<T> createScale(T sx, T sy, T sz)
	{
		Matrix4<T> ret;
		ret.at(0,0) = sx;
		ret.at(1,1) = sy;
		ret.at(2,2) = sz;

		return ret;
	}

	/**
	 * Creates new view matrix to look from specified position @a eyePos to specified position @a centerPos
	 * @param eyePos A position of camera
	 * @param centerPos A position where camera looks-at
	 * @param upDir Direction of up vector
	 * @return Resulting view matrix that looks from and at specific position.
	 */
	static Matrix4<T> createLookAt(const Vector3<T>& eyePos, const Vector3<T>& centerPos, const Vector3<T>& upDir)
	{
		Vector3<T> forward, side, up;
		Matrix4<T> m;

		forward = centerPos - eyePos;
		up = upDir;

		forward.normalize();

		// Side = forward x up
		side = forward.crossProduct(up);
		side.normalize();

		// Recompute up as: up = side x forward
		up = side.crossProduct(forward);

		m.at(0, 0) = side.x;
		m.at(1, 0) = side.y;
		m.at(2, 0) = side.z;

		m.at(0, 1) = up.x;
		m.at(1, 1) = up.y;
		m.at(2, 1) = up.z;

		m.at(0, 2) = -forward.x;
		m.at(1, 2) = -forward.y;
		m.at(2, 2) = -forward.z;

		m = m * Matrix4<T>::createTranslation(-eyePos.x, -eyePos.y, -eyePos.z);
		return m;
	}


	/**
	 * Creates OpenGL compatible perspective projection according specified frustum parameters.
	 *
	 * @param left Specify the coordinate for the left vertical clipping plane,
	 * @param right Specify the coordinate for the right vertical clipping plane.
	 * @param bottom Specify the coordinate for the bottom horizontal clipping plane,
	 * @param top Specify the coordinate for the top horizontal clipping plane.
	 * @param zNear Specify the distance to the near clipping plane.  Distance must be positive.
	 * @param zFar Specify the distance to the far depth clipping plane.  Distance must be positive.
	 *
	 * @return Projection matrix for specified frustum.
	 */
	static Matrix4<T> createFrustum(T left, T right, T bottom, T top, T zNear, T zFar)
	{
		/*
		 *
      	  2 zNear
        ------------       0              A              0
        right - left

                        2 zNear
            0         ------------        B              0
                      top - bottom

            0              0              C              D

            0              0              -1             0

                                                     A = (right + left) / (right - left)

                                                     B = (top + bottom) / (top - bottom)

                                                    C = - (zFar + zNear) / (zFar - zNear)

                                                    D = - (2 zFar zNear) / (zFar - zNear)
		 *
		 */
		Matrix4<T> ret;

		const T invWidth = 1.0 / (right - left);
		const T invHeight = 1.0 / (top - bottom);
		const T invDepth = 1.0 / (zFar - zNear);

		const T twoZNear = 2 * zNear;

		ret.at(0,0) = twoZNear * invWidth;
		ret.at(1,1) = twoZNear * invHeight;

		ret.at(2,0) = (right + left) * invWidth;
		ret.at(2,1) = (top + bottom) * invHeight;
		ret.at(2,2) = - (zFar + zNear) * invDepth;
		ret.at(2,3) = -1;

		ret.at(3,2) = - twoZNear * zFar * invDepth;
		ret.at(3,3) = 0;

		return ret;
	}

	/**
	 * Creates OpenGL compatible orthographic projection matrix.
	 * @param left Specify the coordinate for the left vertical clipping plane,
	 * @param right Specify the coordinate for the right vertical clipping plane.
	 * @param bottom Specify the coordinate for the bottom horizontal clipping plane,
	 * @param top Specify the coordinate for the top horizontal clipping plane.
	 * @param zNear Specify the distance to the nearer depth clipping plane.
	 *       This value is negative if the plane is to be behind the viewer,
	 * @param zFar Specify the distance to the farther depth clipping plane.
	 *       This value is negative if the plane is to be behind the viewer.
	 * @return Othrographic projection matrix.
	 */
	static Matrix4<T> createOrtho(T left, T right, T bottom, T top, T zNear, T zFar)
	{
		/*
		   	   2
        ------------       0              0              tx
        right - left

                           2
            0         ------------        0              ty
                      top - bottom

                                          -2
            0              0         ------------        tz
                                      zFar-zNear

            0              0              0              1

       where

                                                    tx = - (right + left) / (right - left)

                                                    ty = - (top + bottom) / (top - bottom)

                                                    tz = - (zFar + zNear) / (zFar - zNear)

		 */

		const T invWidth = 1.0 / (right  - left);
		const T invHeight = 1.0 / (top - bottom);
		const T invDepth = 1.0 / (zFar - zNear);

		Matrix4<T> ret;

		ret.at(0,0) = 2 * invWidth;
		ret.at(1,1) = 2 * invHeight;
		ret.at(2,2) = -2 * invDepth;

		ret.at(3,0) = -(right + left) * invWidth;
		ret.at(3,1) = -(top + bottom) * invHeight;
		ret.at(3,2) = -(zFar + zNear) * invDepth;

		return ret;
	}

	/**
	 * Creates new matrix 4x4 from array that represents such matrix 4x4
	 * as array of tightly packed elements in row major order.
	 * @param arr An array of elements for 4x4 matrix in row major order.
	 * @return An instance of Matrix4<T> representing @a arr
	 */
	template<class FromT>
	static Matrix4<T> fromRowMajorArray(const FromT* arr)
	{
		const T retData[] =
		{ static_cast<T>(arr[0]), static_cast<T>(arr[4]), static_cast<T>(arr[8]), static_cast<T>(arr[12]),
				static_cast<T>(arr[1]), static_cast<T>(arr[5]), static_cast<T>(arr[9]), static_cast<T>(arr[13]),
				static_cast<T>(arr[2]), static_cast<T>(arr[6]), static_cast<T>(arr[10]), static_cast<T>(arr[14]),
				static_cast<T>(arr[3]), static_cast<T>(arr[7]), static_cast<T>(arr[11]), static_cast<T>(arr[15]) };

		return retData;
	}

	/**
	 * Creates new matrix 4x4 from array that represents such matrix 4x4
	 * as array of tightly packed elements in column major order.
	 * @param arr An array of elements for 4x4 matrix in column major order.
	 * @return An instance of Matrix4<T> representing @a arr
	 */
	template<class FromT>
	static Matrix4<T> fromColumnMajorArray(const FromT* arr)
	{
		const T retData[] =
		{ static_cast<T>(arr[0]), static_cast<T>(arr[1]), static_cast<T>(arr[2]), static_cast<T>(arr[3]),
				static_cast<T>(arr[4]), static_cast<T>(arr[5]), static_cast<T>(arr[6]), static_cast<T>(arr[7]),
				static_cast<T>(arr[8]), static_cast<T>(arr[9]), static_cast<T>(arr[10]), static_cast<T>(arr[11]),
				static_cast<T>(arr[12]), static_cast<T>(arr[13]), static_cast<T>(arr[14]), static_cast<T>(arr[15]) };

		return retData;
	}

	//---------------------[ Equality operators ]------------------------------
	/**
	 * Equality test operator
	 * @param rhs Right hand side argument of binary operator.
	 * @note Test of equality is based of threshold EPSILON value. To be two
	 * values equal, must satisfy this condition all elements of matrix 
	 * | lhs[i] - rhs[i] | < EPSILON,
	 * same for y-coordinate, z-coordinate, and w-coordinate.
	 */
	bool operator==(const Matrix4<T>& rhs) const
	{
		for (int i = 0; i < 16; i++)
		{
			if (std::fabs(data[i] - rhs.data[i]) >= EPSILON
				)
				return false;
		}
		return true;
	}

	/**
	 * Inequality test operator
	 * @param rhs Right hand side argument of binary operator.
	 * @return not (lhs == rhs) :-P
	 */
	bool operator!=(const Matrix4<T>& rhs) const
	{
		return !(*this == rhs);
	}

	//---------------------[ access operators ]---------------------------------
	/**
	 * Get reference to element at postion (x,y).
	 * @param x Number of column (0..3)
	 * @param y Number of row (0..3)
	 */
	T& at(int x, int y)
	{
		assert(x >= 0 && x < 4);
		assert(y >= 0 && y < 4);
		return data[x * 4 + y];
	}

	/**
	 * Get constant reference to element at position (x,y).
	 * @param x Number of column (0..3)
	 * @param y Number of row (0..3)
	 */
	const T& at(int x, int y) const
	{
		assert(x >= 0 && x < 4);
		assert(y >= 0 && y < 4);
		return data[x * 4 + y];
	}

	/**
	 * Get reference to element at position (i,j), with math matrix notation
	 * @param i Number of row (1..4)
	 * @param j Number of column (1..4)
	 */
	T& operator()(int i, int j)
	{
		assert(i >= 1 && i <= 4);
		assert(j >= 1 && j <= 4);
		return data[(j - 1) * 4 + i - 1];
	}

	/**
	 * Get constant reference to element at position (i,j), with math matrix notation
	 * @param i Number of row (1..4)
	 * @param j Number of column (1..4)
	 */
	const T& operator()(int i, int j) const
	{
		assert(i >= 1 && i <= 4);
		assert(j >= 1 && j <= 4);
		return data[(j - 1) * 4 + i - 1];
	}

	/**
	 * Sets translation part of matrix.
	 *
	 * @param v Vector of translation to be set.
	 */
	void setTranslation(const Vector3<T>& v)
	{
		at(3, 0) = v.x;
		at(3, 1) = v.y;
		at(3, 2) = v.z;
		at(3, 3) = 1;
	}

	Vector3<T> getTranslation() const
	{
		return Vector3<T>(at(3, 0), at(3, 1), at(3, 2));
	}

	Matrix3<T> getRotation() const
	{
		Matrix3<T> newMatrix;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				newMatrix.at(i, j) = at(i, j);
			}
		}
		return newMatrix;
	}

	/**
	 * Sets rotation part (matrix 3x3) of matrix.
	 *
	 * @param m Rotation part of matrix
	 */
	void setRotation(const Matrix3<T>& m)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				at(i, j) = m.at(i, j);
			}
		}
	}

	/**
	 * Gets matrix scale
	 * @return Scales (i.e. first three values from matrix diagonal.
	 */
	Vector3<T> getScale() const
	{ return Vector3<T>(at(0,0), at(1,1), at(2,2)); }

	/**
	 * Sets matrix uniform scale values
	 * @param s Uniform scale value
	 */
	void setScale(T s)
	{
		at(0,0) = at(1,1) = at(2,2) = s;
	}

	/**
	 * Sets matrix scale for all axes.
	 * @param sx X-axis scale factor
	 * @param sy Y-axis scale factor
	 * @param sz Z-axis scale factor
	 */
	void setScale(T sx, T sy, T sz)
	{
		at(0,0) = sx;
		at(1,1) = sy;
		at(2,2) = sz;
	}

	/**
	 * Sets matrix scale for all axes.
	 * @param s Scale factors for X, Y, and Z coordinate.
	 */
	void setScale(const Vector3<T>& s)
	{
		at(0,0) = s.x;
		at(1,1) = s.y;
		at(2,2) = s.z;
	}

	/**
	 * Copy operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix4<T>& operator=(const Matrix4<T>& rhs)
	{
		std::memcpy(data, rhs.data, sizeof(T) * 16);
		return *this;
	}

	/**
	 * Copy casting operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	template<class FromT>
	Matrix4<T>& operator=(const Matrix4<FromT>& rhs)
	{
		for (int i = 0; i < 16; i++)
		{
			data[i] = static_cast<T>(rhs.data[i]);
		}
		return *this;
	}

	/**
	 * Copy operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix4<T>& operator=(const T* rhs)
	{
		std::memcpy(data, rhs, sizeof(T) * 16);
		return *this;
	}

	/*Matrix4<T> & operator=(const double* m)
	 {
	 for (int i = 0; i < 16; i++) data[i] = (T)m[i];
	 return * this;
	 }*/

	//--------------------[ matrix with matrix operations ]---------------------
	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix4<T> operator+(const Matrix4<T>& rhs) const
	{
		Matrix4<T> ret;
		for (int i = 0; i < 16; i++)
			ret.data[i] = data[i] + rhs.data[i];
		return ret;
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix4<T> operator-(const Matrix4<T>& rhs) const
	{
		Matrix4<T> ret;
		for (int i = 0; i < 16; i++)
			ret.data[i] = data[i] - rhs.data[i];
		return ret;
	}

	//--------------------[ matrix with scalar operations ]---------------------
	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix4<T> operator+(T rhs) const
	{
		Matrix4<T> ret;
		for (int i = 0; i < 16; i++)
			ret.data[i] = data[i] + rhs;
		return ret;
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix4<T> operator-(T rhs) const
	{
		Matrix4<T> ret;
		for (int i = 0; i < 16; i++)
			ret.data[i] = data[i] - rhs;
		return ret;
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix4<T> operator*(T rhs) const
	{
		Matrix4<T> ret;
		for (int i = 0; i < 16; i++)
			ret.data[i] = data[i] * rhs;
		return ret;
	}

	/**
	 * Division operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix4<T> operator/(T rhs) const
	{
		Matrix4<T> ret;
		for (int i = 0; i < 16; i++)
			ret.data[i] = data[i] / rhs;
		return ret;
	}

	//--------------------[ multiply operators ]--------------------------------
	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector4<T> operator*(const Vector4<T>& rhs) const
	{
		return Vector4<T>(data[0] * rhs.x + data[4] * rhs.y + data[8] * rhs.z + data[12] * rhs.w,
				data[1] * rhs.x + data[5] * rhs.y + data[9] * rhs.z + data[13] * rhs.w,
				data[2] * rhs.x + data[6] * rhs.y + data[10] * rhs.z + data[14] * rhs.w,
				data[3] * rhs.x + data[7] * rhs.y + data[11] * rhs.z + data[15] * rhs.w);

	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Vector3<T> operator*(const Vector3<T>& rhs) const
	{
		return Vector3<T>(data[0] * rhs.x + data[4] * rhs.y + data[8] * rhs.z,
				data[1] * rhs.x + data[5] * rhs.y + data[9] * rhs.z,
				data[2] * rhs.x + data[6] * rhs.y + data[10] * rhs.z);
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Matrix4<T> operator*(Matrix4<T> rhs) const
	{
		Matrix4<T> w;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				T n = 0;
				for (int k = 0; k < 4; k++)
					n += rhs.at(i, k) * at(k, j);
				w.at(i, j) = n;
			}
		}
		return w;

	}

	//---------------------------[ misc operations ]----------------------------

	/**
	 * Computes determinant of matrix
	 * @return Determinant of matrix
	 * @note This function does 3 * 4 * 6 muls, 3 * 6 adds.
	 */
	T det()
	{

		return +at(3, 0) * at(2, 1) * at(1, 2) * at(0, 3) - at(2, 0) * at(3, 1) * at(1, 2) * at(0, 3)
				- at(3, 0) * at(1, 1) * at(2, 2) * at(0, 3) + at(1, 0) * at(3, 1) * at(2, 2) * at(0, 3)

		+ at(2, 0) * at(1, 1) * at(3, 2) * at(0, 3) - at(1, 0) * at(2, 1) * at(3, 2) * at(0, 3)
				- at(3, 0) * at(2, 1) * at(0, 2) * at(1, 3) + at(2, 0) * at(3, 1) * at(0, 2) * at(1, 3)

		+ at(3, 0) * at(0, 1) * at(2, 2) * at(1, 3) - at(0, 0) * at(3, 1) * at(2, 2) * at(1, 3)
				- at(2, 0) * at(0, 1) * at(3, 2) * at(1, 3) + at(0, 0) * at(2, 1) * at(3, 2) * at(1, 3)

		+ at(3, 0) * at(1, 1) * at(0, 2) * at(2, 3) - at(1, 0) * at(3, 1) * at(0, 2) * at(2, 3)
				- at(3, 0) * at(0, 1) * at(1, 2) * at(2, 3) + at(0, 0) * at(3, 1) * at(1, 2) * at(2, 3)

		+ at(1, 0) * at(0, 1) * at(3, 2) * at(2, 3) - at(0, 0) * at(1, 1) * at(3, 2) * at(2, 3)
				- at(2, 0) * at(1, 1) * at(0, 2) * at(3, 3) + at(1, 0) * at(2, 1) * at(0, 2) * at(3, 3)

		+ at(2, 0) * at(0, 1) * at(1, 2) * at(3, 3) - at(0, 0) * at(2, 1) * at(1, 2) * at(3, 3)
				- at(1, 0) * at(0, 1) * at(2, 2) * at(3, 3) + at(0, 0) * at(1, 1) * at(2, 2) * at(3, 3);

	}

	/**
	 * Computes inverse matrix
	 * @return Inverse matrix of this matrix.
	 * @note This is a little bit time consuming operation
	 * (16 * 6 * 3 muls, 16 * 5 adds + det() + mul() functions)
	 */
	Matrix4<T> inverse()
	{
		Matrix4<T> ret;

		ret.at(0, 0) = +at(2, 1) * at(3, 2) * at(1, 3) - at(3, 1) * at(2, 2) * at(1, 3) + at(3, 1) * at(1, 2) * at(2, 3)
				- at(1, 1) * at(3, 2) * at(2, 3) - at(2, 1) * at(1, 2) * at(3, 3) + at(1, 1) * at(2, 2) * at(3, 3);

		ret.at(1, 0) = +at(3, 0) * at(2, 2) * at(1, 3) - at(2, 0) * at(3, 2) * at(1, 3) - at(3, 0) * at(1, 2) * at(2, 3)
				+ at(1, 0) * at(3, 2) * at(2, 3) + at(2, 0) * at(1, 2) * at(3, 3) - at(1, 0) * at(2, 2) * at(3, 3);

		ret.at(2, 0) = +at(2, 0) * at(3, 1) * at(1, 3) - at(3, 0) * at(2, 1) * at(1, 3) + at(3, 0) * at(1, 1) * at(2, 3)
				- at(1, 0) * at(3, 1) * at(2, 3) - at(2, 0) * at(1, 1) * at(3, 3) + at(1, 0) * at(2, 1) * at(3, 3);

		ret.at(3, 0) = +at(3, 0) * at(2, 1) * at(1, 2) - at(2, 0) * at(3, 1) * at(1, 2) - at(3, 0) * at(1, 1) * at(2, 2)
				+ at(1, 0) * at(3, 1) * at(2, 2) + at(2, 0) * at(1, 1) * at(3, 2) - at(1, 0) * at(2, 1) * at(3, 2);

		ret.at(0, 1) = +at(3, 1) * at(2, 2) * at(0, 3) - at(2, 1) * at(3, 2) * at(0, 3) - at(3, 1) * at(0, 2) * at(2, 3)
				+ at(0, 1) * at(3, 2) * at(2, 3) + at(2, 1) * at(0, 2) * at(3, 3) - at(0, 1) * at(2, 2) * at(3, 3);

		ret.at(1, 1) = +at(2, 0) * at(3, 2) * at(0, 3) - at(3, 0) * at(2, 2) * at(0, 3) + at(3, 0) * at(0, 2) * at(2, 3)
				- at(0, 0) * at(3, 2) * at(2, 3) - at(2, 0) * at(0, 2) * at(3, 3) + at(0, 0) * at(2, 2) * at(3, 3);

		ret.at(2, 1) = +at(3, 0) * at(2, 1) * at(0, 3) - at(2, 0) * at(3, 1) * at(0, 3) - at(3, 0) * at(0, 1) * at(2, 3)
				+ at(0, 0) * at(3, 1) * at(2, 3) + at(2, 0) * at(0, 1) * at(3, 3) - at(0, 0) * at(2, 1) * at(3, 3);

		ret.at(3, 1) = +at(2, 0) * at(3, 1) * at(0, 2) - at(3, 0) * at(2, 1) * at(0, 2) + at(3, 0) * at(0, 1) * at(2, 2)
				- at(0, 0) * at(3, 1) * at(2, 2) - at(2, 0) * at(0, 1) * at(3, 2) + at(0, 0) * at(2, 1) * at(3, 2);

		ret.at(0, 2) = +at(1, 1) * at(3, 2) * at(0, 3) - at(3, 1) * at(1, 2) * at(0, 3) + at(3, 1) * at(0, 2) * at(1, 3)
				- at(0, 1) * at(3, 2) * at(1, 3) - at(1, 1) * at(0, 2) * at(3, 3) + at(0, 1) * at(1, 2) * at(3, 3);

		ret.at(1, 2) = +at(3, 0) * at(1, 2) * at(0, 3) - at(1, 0) * at(3, 2) * at(0, 3) - at(3, 0) * at(0, 2) * at(1, 3)
				+ at(0, 0) * at(3, 2) * at(1, 3) + at(1, 0) * at(0, 2) * at(3, 3) - at(0, 0) * at(1, 2) * at(3, 3);

		ret.at(2, 2) = +at(1, 0) * at(3, 1) * at(0, 3) - at(3, 0) * at(1, 1) * at(0, 3) + at(3, 0) * at(0, 1) * at(1, 3)
				- at(0, 0) * at(3, 1) * at(1, 3) - at(1, 0) * at(0, 1) * at(3, 3) + at(0, 0) * at(1, 1) * at(3, 3);

		ret.at(3, 2) = +at(3, 0) * at(1, 1) * at(0, 2) - at(1, 0) * at(3, 1) * at(0, 2) - at(3, 0) * at(0, 1) * at(1, 2)
				+ at(0, 0) * at(3, 1) * at(1, 2) + at(1, 0) * at(0, 1) * at(3, 2) - at(0, 0) * at(1, 1) * at(3, 2);

		ret.at(0, 3) = +at(2, 1) * at(1, 2) * at(0, 3) - at(1, 1) * at(2, 2) * at(0, 3) - at(2, 1) * at(0, 2) * at(1, 3)
				+ at(0, 1) * at(2, 2) * at(1, 3) + at(1, 1) * at(0, 2) * at(2, 3) - at(0, 1) * at(1, 2) * at(2, 3);

		ret.at(1, 3) = +at(1, 0) * at(2, 2) * at(0, 3) - at(2, 0) * at(1, 2) * at(0, 3) + at(2, 0) * at(0, 2) * at(1, 3)
				- at(0, 0) * at(2, 2) * at(1, 3) - at(1, 0) * at(0, 2) * at(2, 3) + at(0, 0) * at(1, 2) * at(2, 3);

		ret.at(2, 3) = +at(2, 0) * at(1, 1) * at(0, 3) - at(1, 0) * at(2, 1) * at(0, 3) - at(2, 0) * at(0, 1) * at(1, 3)
				+ at(0, 0) * at(2, 1) * at(1, 3) + at(1, 0) * at(0, 1) * at(2, 3) - at(0, 0) * at(1, 1) * at(2, 3);

		ret.at(3, 3) = +at(1, 0) * at(2, 1) * at(0, 2) - at(2, 0) * at(1, 1) * at(0, 2) + at(2, 0) * at(0, 1) * at(1, 2)
				- at(0, 0) * at(2, 1) * at(1, 2) - at(1, 0) * at(0, 1) * at(2, 2) + at(0, 0) * at(1, 1) * at(2, 2);

		return ret / det();
	}

	/**
	 * Transpose matrix.
	 */
	Matrix4<T> transpose()
	{
		Matrix4<T> ret;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				ret.at(i, j) = at(j, i);
			}
		}
		return ret;
	}

	/**
	 * Linear interpolation of two matrices
	 * @param fact Factor of interpolation. For translation from positon
	 * of this matrix (lhs) to matrix rhs, values of factor goes from 0.0 to 1.0.
	 * @param rhs Second Matrix for interpolation 
	 * @note However values of fact parameter are reasonable only in interval
	 * [0.0 , 1.0], you can pass also values outside of this interval and you
	 * can get result (extrapolation?)
	 */
	Matrix4<T> lerp(T fact, const Matrix4<T>& rhs) const
	{
		Matrix4<T> ret = (*this) + (rhs - (*this)) * fact;
		return ret;
	}

	//-------------[ conversion ]-----------------------------
	/**
	 * Conversion to pointer operator
	 * @return Pointer to internally stored (in management of class Matrix4<T>)
	 * used for passing Matrix4<T> values to gl*[fd]v functions.
	 */
	operator T*()
	{
		return (T*) data;
	}

	/**
	 * Conversion to pointer operator
	 * @return Constant Pointer to internally stored (in management of class Matrix4<T>)
	 * used for passing Matrix4<T> values to gl*[fd]v functions.
	 */
	operator const T*() const
	{
		return (const T*) data;
	}

	//----------[ output operator ]----------------------------
	/**
	 * Output to stream operator
	 * @param lhs Left hand side argument of operator (commonly ostream instance). 
	 * @param rhs Right hand side argument of operator.
	 * @return Left hand side argument - the ostream object passed to operator.
	 */
	friend std::ostream& operator <<(std::ostream& lhs, const Matrix4<T>& rhs)
	{
		for (int i = 0; i < 4; i++)
		{
			lhs << "|\t";
			for (int j = 0; j < 4; j++)
			{
				lhs << rhs.at(j, i) << "\t";
			}
			lhs << "|" << std::endl;
		}
		return lhs;
	}

	/**
	 * Gets string representation.
	 */
	std::string toString() const
	{
		std::ostringstream oss;
		oss << *this;
		return oss.str();
	}

};

/// Matrix 4x4 of floats
typedef Matrix4<float> Matrix4f;
/// Matrix 4x4 of doubles
typedef Matrix4<double> Matrix4d;
/// Matrix 4x4 of int
typedef Matrix4<int> Matrix4i;

/**
 * Quaternion class implementing some quaternion algebra operations.
 * Quaternion is kind of complex number it consists of its real part (w)
 * and its complex part v. This complex part has three elements, so we
 * can express it as xi + yj + zk . Note that coordinates of (x,y,z) are
 * hold inside v field.
 */
template<class T>
class Quaternion
{
public:
	/**
	 * Imaginary part of quaternion.
	 */
	Vector3<T> v;
	/**
	 * Real part of quaternion.
	 */
	T w;

	/**
	 * Quaternion constructor, sets quaternion to (0 + 0i + 0j + 0k).
	 */
	Quaternion()
			: w(0), v(0, 0, 0)
	{
	}

	/**
	 * Copy constructor.
	 */
	Quaternion(const Quaternion<T>& q)
			: w(q.w), v(q.v)
	{
	}

	/**
	 * Copy casting constructor.
	 */
	template<class FromT>
	Quaternion(const Quaternion<FromT>& q)
			: w(static_cast<T>(q.w)), v(q.v)
	{
	}

	/**
	 * Creates quaternion object from real part w_ and complex part v_.
	 * @param w_ Real part of quaternion.
	 * @param v_ Complex part of quaternion (xi + yj + zk).
	 */
	Quaternion(T w_, const Vector3<T>& v_)
			: w(w_), v(v_)
	{
	}

	/**
	 * Creates quaternion object from value (w_ + xi + yj + zk).
	 * @param w_ Real part of quaternion.
	 * @param x Complex coefficient for i complex constant.
	 * @param y Complex coefficient for j complex constant.
	 * @param z Complex coefficient for k complex constant.
	 */
	Quaternion(T w_, T x, T y, T z)
			: w(w_), v(x, y, z)
	{
	}

	/**
	 * Copy operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Quaternion<T>& operator=(const Quaternion<T>& rhs)
	{
		v = rhs.v;
		w = rhs.w;
		return *this;
	}

	/**
	 * Copy convert operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	template<class FromT>
	Quaternion<T>& operator=(const Quaternion<FromT>& rhs)
	{
		v = rhs.v;
		w = static_cast<T>(rhs.w);
		return *this;
	}

	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Quaternion<T> operator+(const Quaternion<T>& rhs) const
	{
		const Quaternion<T>& lhs = *this;
		return Quaternion<T>(lhs.w + rhs.w, lhs.v + rhs.v);
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Quaternion<T> operator*(const Quaternion<T>& rhs) const
	{
		const Quaternion<T>& lhs = *this;
		return Quaternion<T>(lhs.w * rhs.w - lhs.v.x * rhs.v.x - lhs.v.y * rhs.v.y - lhs.v.z * rhs.v.z,
				lhs.w * rhs.v.x + lhs.v.x * rhs.w + lhs.v.y * rhs.v.z - lhs.v.z * rhs.v.y,
				lhs.w * rhs.v.y - lhs.v.x * rhs.v.z + lhs.v.y * rhs.w + lhs.v.z * rhs.v.x,
				lhs.w * rhs.v.z + lhs.v.x * rhs.v.y - lhs.v.y * rhs.v.x + lhs.v.z * rhs.w);
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Quaternion<T> operator*(T rhs) const
	{
		return Quaternion<T>(w * rhs, v * rhs);
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Quaternion<T> operator-(const Quaternion<T>& rhs) const
	{
		const Quaternion<T>& lhs = *this;
		return Quaternion<T>(lhs.w - rhs.w, lhs.v - rhs.v);
	}

	/**
	 * Addition operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Quaternion<T>& operator+=(const Quaternion<T>& rhs)
	{
		w += rhs.w;
		v += rhs.v;
		return *this;
	}

	/**
	 * Subtraction operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Quaternion<T>& operator-=(const Quaternion<T>& rhs)
	{
		w -= rhs.w;
		v -= rhs.v;
		return *this;
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Quaternion<T>& operator*=(const Quaternion<T>& rhs)
	{
		Quaternion q = (*this) * rhs;
		v = q.v;
		w = q.w;
		return *this;
	}

	/**
	 * Multiplication operator
	 * @param rhs Right hand side argument of binary operator.
	 */
	Quaternion<T>& operator*=(T rhs)
	{
		w *= rhs;
		v *= rhs;
		return *this;
	}

	/**
	 * Equality test operator
	 * @param rhs Right hand side argument of binary operator.
	 * @note Test of equality is based of threshold EPSILON value. To be two
	 * values equal, must satisfy this condition | lhs - rhs | < EPSILON,
	 * for all quaternion coordinates.
	 */
	bool operator==(const Quaternion<T>& rhs) const
	{
		const Quaternion<T>& lhs = *this;
		return (std::fabs(lhs.w - rhs.w) < EPSILON) && lhs.v == rhs.v;
	}

	/**
	 * Inequality test operator
	 * @param rhs Right hand side argument of binary operator.
	 * @return not (lhs == rhs) :-P
	 */
	bool operator!=(const Quaternion<T>& rhs) const
	{
		return !(*this == rhs);
	}

	//-------------[ unary operations ]--------------------------
	/**
	 * Unary negate operator
	 * @return negated quaternion
	 */
	Quaternion<T> operator-() const
	{
		return Quaternion<T>(-w, -v);
	}

	/**
	 * Unary conjugate operator
	 * @return conjugated quaternion
	 */
	Quaternion<T> operator~() const
	{
		return Quaternion<T>(w, -v);
	}

	/**
	 * Get lenght of quaternion.
	 * @return Length of quaternion.
	 */
	T length() const
	{
		return (T) std::sqrt(w * w + v.lengthSq());
	}

	/**
	 * Return square of length.
	 * @return length ^ 2
	 * @note This method is faster then length(). For comparison
	 * of length of two quaternion can be used just this value, instead
	 * of more expensive length() method.
	 */
	T lengthSq() const
	{
		return w * w + v.lengthSq();
	}

	/**
	 * Normalize quaternion
	 */
	void normalize()
	{
		T len = length();
		w /= len;
		v /= len;
	}

	/**
	 * Creates quaternion for eulers angles.
	 * @param x Rotation around x axis (in degrees).
	 * @param y Rotation around y axis (in degrees).
	 * @param z Rotation around z axis (in degrees).
	 * @return Quaternion object representing transformation.
	 */
	static Quaternion<T> fromEulerAngles(T x, T y, T z)
	{
		Quaternion<T> ret = fromAxisRot(Vector3<T>(1, 0, 0), x) * fromAxisRot(Vector3<T>(0, 1, 0), y)
				* fromAxisRot(Vector3<T>(0, 0, 1), z);
		return ret;
	}

	/**
	 * Creates quaternion as rotation around axis.
	 * @param axis Unit vector expressing axis of rotation.
	 * @param angleDeg Angle of rotation around axis (in degrees).
	 */
	static Quaternion<T> fromAxisRot(Vector3<T> axis, float angleDeg)
	{
		double angleRad = DEG2RAD(angleDeg);
		double sa2 = std::sin(angleRad / 2);
		double ca2 = std::cos(angleRad / 2);
		return Quaternion<T>(ca2, axis * sa2);
	}

	/**
	 * Converts quaternion into rotation matrix.
	 * @return Rotation matrix expressing this quaternion.
	 */
	Matrix3<T> rotMatrix()
	{
		Matrix3<T> ret;

		/*ret.at(0,0) = 1 - 2*v.y*v.y - 2*v.z*v.z;
		 ret.at(1,0) = 2*v.x*v.y - 2*w*v.z;
		 ret.at(2,0) = 2*v.x*v.z - 2*w*v.y;
		 
		 ret.at(0,1) = 2*v.x*v.y + 2*w*v.z;
		 ret.at(1,1) = 1 - 2*v.x*v.x - 2*v.z*v.z;
		 ret.at(2,1) = 2*v.y*v.z - 2*w*v.x;
		 
		 ret.at(0,2) = 2*v.x*v.z - 2*w*v.y;
		 ret.at(1,2) = 2*v.y*v.z + 2*w*v.x;
		 ret.at(2,2) = 1 - 2*v.x*v.x - 2*v.y*v.y;*/

		T xx = v.x * v.x;
		T xy = v.x * v.y;
		T xz = v.x * v.z;
		T xw = v.x * w;

		T yy = v.y * v.y;
		T yz = v.y * v.z;
		T yw = v.y * w;

		T zz = v.z * v.z;
		T zw = v.z * w;

		ret.at(0, 0) = 1 - 2 * (yy + zz);
		ret.at(1, 0) = 2 * (xy - zw);
		ret.at(2, 0) = 2 * (xz + yw);

		ret.at(0, 1) = 2 * (xy + zw);
		ret.at(1, 1) = 1 - 2 * (xx + zz);
		ret.at(2, 1) = 2 * (yz - xw);

		ret.at(0, 2) = 2 * (xz - yw);
		ret.at(1, 2) = 2 * (yz + xw);
		ret.at(2, 2) = 1 - 2 * (xx + yy);

		return ret;
	}

	/**
	 * Converts quaternion into transformation matrix.
	 * @note This method performs same operation as rotMatrix() 
	 * conversion method. But returns Matrix of 4x4 elements.
	 * @return Transformation matrix expressing this quaternion.
	 */
	Matrix4<T> transform() const
	{
		Matrix4<T> ret;

		T xx = v.x * v.x;
		T xy = v.x * v.y;
		T xz = v.x * v.z;
		T xw = v.x * w;

		T yy = v.y * v.y;
		T yz = v.y * v.z;
		T yw = v.y * w;

		T zz = v.z * v.z;
		T zw = v.z * w;

		ret.at(0, 0) = 1 - 2 * (yy + zz);
		ret.at(1, 0) = 2 * (xy - zw);
		ret.at(2, 0) = 2 * (xz + yw);
		ret.at(3, 0) = 0;

		ret.at(0, 1) = 2 * (xy + zw);
		ret.at(1, 1) = 1 - 2 * (xx + zz);
		ret.at(2, 1) = 2 * (yz - xw);
		ret.at(3, 1) = 0;

		ret.at(0, 2) = 2 * (xz - yw);
		ret.at(1, 2) = 2 * (yz + xw);
		ret.at(2, 2) = 1 - 2 * (xx + yy);
		ret.at(3, 2) = 0;

		ret.at(0, 3) = 0;
		ret.at(1, 3) = 0;
		ret.at(2, 3) = 0;
		ret.at(3, 3) = 1;

		return ret;

	}

	/**
	 *
	 */
	Quaternion<T> conjugated() const
	{
		return Quaternion<T>(w, -v.x, -v.y, -v.z);
	}

	/**
	 * Linear interpolation of two quaternions
	 * @param fact Factor of interpolation. For translation from position
	 * of this vector to quaternion rhs, values of factor goes from 0.0 to 1.0.
	 * @param rhs Second Quaternion for interpolation 
	 * @note However values of fact parameter are reasonable only in interval
	 * [0.0 , 1.0], you can pass also values outside of this interval and you
	 * can get result (extrapolation?)
	 */
	Quaternion<T> lerp(T fact, const Quaternion<T>& rhs) const
	{
		return Quaternion<T>((1 - fact) * w + fact * rhs.w, v.lerp(fact, rhs.v));
	}

	/**
	 * Provides output to standard output stream.
	 */
	friend std::ostream& operator <<(std::ostream& oss, const Quaternion<T>& q)
	{
		oss << "Re: " << q.w << " Im: " << q.v;
		return oss;
	}

	/**
	 * Gets string representation.
	 */
	std::string toString() const
	{
		std::ostringstream oss;
		oss << *this;
		return oss.str();
	}

	/**
	 * Creates quaternion from transform matrix.
	 *
	 * @param m Transform matrix used to compute quaternion.
	 * @return Quaternion representing rotation of matrix m.
	 */
    // 2011-07-02: Davide Bacchet: changed formula to fix degenerate cases
	 static Quaternion<T> fromMatrix(const Matrix4<T>& m)
	 {
		Quaternion<T> q;

		T tr, s;
		tr = m(1, 1) + m(2, 2) + m(3, 3);
		if (tr >= epsilon)
		{
			s = 0.5 / (T) sqrt(tr + 1.0);
			q.w = 0.25 / s;
			q.v.x = (m(3, 2) - m(2, 3)) * s;
			q.v.y = (m(1, 3) - m(3, 1)) * s;
			q.v.z = (m(2, 1) - m(1, 2)) * s;
		}
		else
		{
			T d0 = m(1, 1);
			T d1 = m(2, 2);
			T d2 = m(3, 3);

			char bigIdx = (d0 > d1) ? ((d0 > d2) ? 0 : 2):((d1 > d2) ? 1 : 2);

			if (bigIdx == 0)
			{
				s = 2.0 * (T) sqrt(1.0 + m(1, 1) - m(2, 2) - m(3, 3));
				q.w = (m(3, 2) - m(2, 3)) / s;
				q.v.x = 0.25 * s;
				q.v.y = (m(1, 2) + m(2, 1)) / s;
				q.v.z = (m(1, 3) + m(3, 1)) / s;
			}
			else if (bigIdx == 1)
			{
				s = 2.0 * (T) sqrt(1.0 + m(2, 2) - m(1, 1) - m(3, 3));
				q.w = (m(1, 3) - m(3, 1)) / s;
				q.v.x = (m(1, 2) + m(2, 1)) / s;
				q.v.y = 0.25 * s;
				q.v.z = (m(2, 3) + m(3, 2)) / s;
			}
			else
			{
				s = 2.0 * (T) sqrt(1.0 + m(3, 3) - m(1, 1) - m(2, 2));
				q.w = (m(2, 1) - m(1, 2)) / s;
				q.v.x = (m(1, 3) + m(3, 1)) / s;
				q.v.y = (m(2, 3) + m(3, 2)) / s;
				q.v.z = 0.25 * s;
			}
		}

		return q;
	}

	/**
	 * Creates quaternion from rotation matrix.
	 *
	 * @param m Rotation matrix used to compute quaternion.
	 * @return Quaternion representing rotation of matrix m.
	 */

     // 2011-07-02: Davide Bacchet: changed formula to fix degenerate cases
	 static Quaternion<T> fromMatrix(const Matrix3<T>& m)
	 {
		Quaternion<T> q;

		T tr, s;
		tr = m(1, 1) + m(2, 2) + m(3, 3);
		if (tr >= epsilon)
		{
			s = (T) 0.5 / (T) sqrt(tr + 1.0);
			q.w = (T) 0.25 / s;
			q.v.x = (m(3, 2) - m(2, 3)) * s;
			q.v.y = (m(1, 3) - m(3, 1)) * s;
			q.v.z = (m(2, 1) - m(1, 2)) * s;
		}
		else
		{
			T d0 = m(1, 1);
			T d1 = m(2, 2);
			T d2 = m(3, 3);

			char bigIdx = (d0 > d1) ? ((d0 > d2) ? 0 : 2):((d1 > d2) ? 1 : 2);

			if (bigIdx == 0)
			{
				s = (T) 2.0 * (T) sqrt(1.0 + m(1, 1) - m(2, 2) - m(3, 3));
				q.w = (m(3, 2) - m(2, 3)) / s;
				q.v.x = (T)0.25 * s;
				q.v.y = (m(1, 2) + m(2, 1)) / s;
				q.v.z = (m(1, 3) + m(3, 1)) / s;
			}
			else if (bigIdx == 1)
			{
				s = (T)2.0 * (T) sqrt(1.0 + m(2, 2) - m(1, 1) - m(3, 3));
				q.w = (m(1, 3) - m(3, 1)) / s;
				q.v.x = (m(1, 2) + m(2, 1)) / s;
				q.v.y = 0.25 * s;
				q.v.z = (m(2, 3) + m(3, 2)) / s;
			}
			else
			{
				s = 2.0 * (T) sqrt(1.0 + m(3, 3) - m(1, 1) - m(2, 2));
				q.w = (m(2, 1) - m(1, 2)) / s;
				q.v.x = (m(1, 3) + m(3, 1)) / s;
				q.v.y = (m(2, 3) + m(3, 2)) / s;
				q.v.z = 0.25 * s;
			}
		}

		return q;
	}

	/**
	 * Computes spherical interpolation between quaternions (this, q2)
	 * using coefficient of interpolation r (in [0, 1]).
	 *
	 * @param r The ratio of interpolation form this (r = 0) to q2 (r = 1).
	 * @param q2 Second quaternion for interpolation.
	 * @return Result of interpolation.
	 */
	Quaternion<T> slerp(T r, const Quaternion<T>& q2) const
	{
		Quaternion<T> ret;
		T cosTheta = w * q2.w + v.x * q2.v.x + v.y * q2.v.y + v.z * q2.v.z;
		T theta = (T) acos(cosTheta);
		if (fabs(theta) < epsilon)
		{
			ret = *this;
		}
		else
		{
			T sinTheta = (T) sqrt(1.0 - cosTheta * cosTheta);
			if (fabs(sinTheta) < epsilon)
			{
				ret.w = 0.5 * w + 0.5 * q2.w;
				ret.v = v.lerp(0.5, q2.v);
			}
			else
			{
				T rA = (T) sin((1.0 - r) * theta) / sinTheta;
				T rB = (T) sin(r * theta) / sinTheta;

				ret.w = w * rA + q2.w * rB;
				ret.v.x = v.x * rA + q2.v.x * rB;
				ret.v.y = v.y * rA + q2.v.y * rB;
				ret.v.z = v.z * rA + q2.v.z * rB;
			}
		}
		return ret;
	}

};

typedef Quaternion<float> Quatf;
typedef Quaternion<double> Quatd;

#ifdef VMATH_NAMESPACE
}
#endif



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Standard C++ library extensions
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Shortcut defines
#ifdef VMATH_NAMESPACE
#define VEC2 VMATH_NAMESPACE::Vector2
#define VEC3 VMATH_NAMESPACE::Vector3
#define VEC4 VMATH_NAMESPACE::Vector4
#else
#define VEC2 Vector2
#define VEC3 Vector3
#define VEC4 Vector4
#endif
//
//namespace std
//{
//
//	/**
//	 * Gets vector containing minimal values of @a a and @a b coordinates.
//	 * @return Vector of minimal coordinates.
//	 */
//	template <typename T>
//	VEC2<T> min(const VEC2<T>& a, const VEC2<T>& b)
//	{
//		return VEC2<T>(::std::min(a.x, b.x), ::std::min(a.y, b.y));
//	}
//
//	/**
//	 * Gets vector containing minimal values of @a a and @a b coordinates.
//	 * @return Vector of minimal coordinates.
//	 */
//	template <typename T>
//	VEC3<T> min(const VEC3<T>& a, const VEC3<T>& b)
//	{
//		return VEC3<T>(::std::min(a.x, b.x), ::std::min(a.y, b.y), ::std::min(a.z, b.z));
//	}
//
//	/**
//	 * Gets vector containing minimal values of @a a and @a b coordinates.
//	 * @return Vector of minimal coordinates.
//	 */
//	template <typename T>
//	VEC4<T> min(const VEC4<T>& a, const VEC4<T>& b)
//	{
//		return VEC4<T>(::std::min(a.x, b.x), ::std::min(a.y, b.y), ::std::min(a.z, b.z), ::std::min(a.w, b.w));
//	}
//
//	/**
//	 * Gets vector containing maximal values of @a a and @a b coordinates.
//	 * @return Vector of maximal coordinates.
//	 */
//	template <typename T>
//	VEC2<T> max(const VEC2<T>& a, const VEC2<T>& b)
//	{
//		return VEC2<T>(::std::max(a.x, b.x), ::std::max(a.y, b.y));
//	}
//
//	/**
//	 * Gets vector containing maximal values of @a a and @a b coordinates.
//	 * @return Vector of maximal coordinates.
//	 */
//	template <typename T>
//	VEC3<T> max(const VEC3<T>& a, const VEC3<T>& b)
//	{
//		return VEC3<T>(::std::max(a.x, b.x), ::std::max(a.y, b.y), ::std::max(a.z, b.z));
//	}
//
//	/**
//	 * Gets vector containing maximal values of @a a and @a b coordinates.
//	 * @return Vector of maximal coordinates.
//	 */
//	template <typename T>
//	VEC4<T> max(const VEC4<T>& a, const VEC4<T>& b)
//	{
//		return VEC4<T>(::std::max(a.x, b.x), ::std::max(a.y, b.y), ::std::max(a.z, b.z), ::std::max(a.w, b.w));
//	}
//}

// cleanup shortcut defines
#undef VEC2
#undef VEC3
#undef VEC4

#ifdef VMATH_NAMESPACE
namespace VMATH_NAMESPACE
{
#endif //VMATH_NAMESPACE

	///**
	// * Axes-aligned bounding-box (aka AABB) class.
	// *
	// * This class provides functionality for:
	// * - creating AABB from point, or other AABB,
	// * - testing if point of other AABB intersects with,
	// * - getting result of intersection with other AABB,
	// * - transforming AABB with 4x4 matrix.
	// *
	// * There are also overloaded couple of operators to shorten common operation.
	// * For instance you can use  @c operator<< on AABB to extend it with passed point or other AABB.
	// * @code
	// * Aabb3f aabb;
	// * aabb << Vector3f(1, 1, 2) << Aabb3f(-3,-3,-3, 2, 2, 2);
	// * @endcode
	// */
	//template <typename T>
	//class Aabb3
	//{
	//public:
	//	/**
	//	 * Position of Min corner of bounding box.
	//	 */
	//	Vector3<T> min;

	//	/**
	//	 * Position of Max corner of bounding box
	//	 */
	//	Vector3<T> max;

	//	/**
	//	 * Constructs invalid axes-aligned bounding-box.
	//	 * @see valid() for explanation of invalid bounding-box usage.
	//	 */
	//	Aabb3()
	//		: min(1,1,1), max(-1,-1,-1)
	//	{}

	//	/**
	//	 * Constructs axes-aligned bound-box containing one point @a point
	//	 * @param point
	//	 */
	//	template <typename SrcT>
	//	Aabb3(const Vector3<SrcT>& point)
	//		: min(point), max(point)
	//	{}

	//	/**
	//	 * Constructs axes-aligned bounding-box form two corner points (@a x0, @a y0, @a z0)
	//	 * and (@a x1, @a y1, @a z1)
	//	 * @param x0 X-coordinate of first point
	//	 * @param y0 Y-coordinate of first point
	//	 * @param z0 Z-coordinate of first point
	//	 * @param x1 X-coordinate of second point
	//	 * @param y1 Y-coordinate of second point
	//	 * @param z1 Z-coordinate of second point
	//	 */
	//	template <typename SrcT>
	//	Aabb3(SrcT x0, SrcT y0, SrcT z0, SrcT x1, SrcT y1, SrcT z1)
	//		: min(std::min(x0,x1), std::min(y0,y1), std::min(z0,z1)),
	//		  max(std::max(x0,x1), std::max(y0,y1), std::max(z0,z1))
	//	{}

	//	/**
	//	 * Constructs axes-aligned bounding-box containing point (@a x, @a y, @a z)
	//	 * @param x X-coordinate of point
	//	 * @param y Y-coordinate of point
	//	 * @param z Z-coordinate of point
	//	 */
	//	template <typename SrcT>
	//	Aabb3(SrcT x, SrcT y, SrcT z)
	//		: min(x,y,z), max(x,y,z)
	//	{}

	//	/**
	//	 * Creates copy of axis-aligned bounding-box
	//	 * @param src Source bounding-box
	//	 */
	//	template <typename SrcT>
	//	Aabb3(const Aabb3<SrcT>& src)
	//		: min(src.min), max(src.max)
	//	{}

	//	/**
	//	 * Assign operator
	//	 * @param rhs source bounding-box
	//	 * @return refenrence to this
	//	 */
	//	template <typename SrcT>
	//	Aabb3<T>& operator=(const Aabb3<SrcT>& rhs)
	//	{
	//		min = rhs.min;
	//		max = rhs.max;
	//		return *this;
	//	}


	//	/**
	//	 * Checks if bounding-box is valid. Valid bounding-box has non-negative size.
	//	 * If an invalid bounding-box is extended by point or another bounding-box, the target
	//	 * bounding box becomes valid and contains solely the source point or bounding-box respectively.
	//	 * @return True if box is valid, otherwise false
	//	 */
	//	bool valid() const
	//	{ return min.x <= max.x && min.y <= max.y && min.z <= max.z; }

	//	/**
	//	 * Makes this bounding-box invalid. So calling valid() gets false.
	//	 * @see valid() method for more info on usage of invalid bounding-boxes.
	//	 */
	//	void invalidate()
	//	{ min = Vector3<T>(1,1,1); max = Vector3<T>(-1,-1,-1); }

	//	/**
	//	 * Extends this bounding-box by a point @a point.
	//	 * @param point A point to extend bounding-box by.
	//	 */
	//	template <typename SrcT>
	//	void extend(const Vector3<SrcT>& point)
	//	{
	//		if (!valid())
	//		{
	//			min = max = point;
	//		}
	//		else
	//		{
	//			min = std::min(min, point);
	//			max = std::max(max, point);
	//		}
	//	}

	//	/**
	//	 * Extends this bounding-box by a box @a box.
	//	 * @param box A box to extend this bounding-box by.
	//	 */
	//	template <typename SrcT>
	//	void extend(const Aabb3<SrcT>& box)
	//	{
	//		if (!valid())
	//		{
	//			min = box.min;
	//			max = box.max;
	//		}
	//		else
	//		{
	//			min = std::min(min, box.min);
	//			max = std::max(max, box.max);
	//		}
	//	}

	//	/**
	//	 * Gets a copy of this bounding-box extend by a point @a point.
	//	 * @param point A point to extend the box by
	//	 * @return Copy of extended bounding-box
	//	 */
	//	template <typename SrcT>
	//	Aabb3<T> extended(const Vector3<SrcT>& point) const
	//	{
	//		Aabb3<T> ret(*this);
	//		ret.extend(point);
	//		return ret;
	//	}

	//	/**
	//	 * Gets a copy of this bounding-box extnended by box @a box.
	//	 * @param box A box to extend the copy be.
	//	 * @return Copy of extended bounding-box
	//	 */
	//	template <typename SrcT>
	//	Aabb3<T> extended(const Aabb3<SrcT>& box) const
	//	{
	//		Aabb3<T> ret(*this);
	//		ret.extend(box);
	//		return ret;
	//	}

	//	/**
	//	 * Tests if the point @a point is within this bounding-box
	//	 * @param point A point to be tested
	//	 * @return True if point @a point lies within bounding-box, otherwise false.
	//	 */
	//	template <typename SrcT>
	//	bool intersects(const Vector3<SrcT>& point) const
	//	{
	//		/*
	//		for (size_t i = 0; i < 3; i++)
	//		{
	//			if (min[i] > point[i] || point[i] > max[i])
	//				return false;
	//		}
	//		*/
	//		// unrolled loop above
	//		if (min.x > point.x || point.x > max.x)
	//			return false;
	//		if (min.y > point.y || point.y > max.y)
	//			return false;
	//		if (min.z > point.z || point.z > max.z)
	//			return false;

	//		return true;
	//	}

	//	/**
	//	 * Tests if other bounding-box @a box intersects (even partially) with this bouding-box.
	//	 * @param box A box to be tested for intersection.
	//	 * @return True if there's intersection between boxes, otherwise false.
	//	 */
	//	template <typename SrcT>
	//	bool intersects(const Aabb3<SrcT>& box) const
	//	{
	//		/*
	//		for (size_t i = 0; i < 3; i++)
	//		{
	//			if (max[i] < box.min[i] || min[i] > box.max[i])
	//				return false;
	//		}
	//		*/
	//		// unrolled loop above
	//		if (max.x < box.min.x || min.x > box.max.x)
	//			return false;
	//		if (max.y < box.min.y || min.y > box.max.y)
	//			return false;
	//		if (max.z < box.min.z || min.z > box.max.z)
	//			return false;

	//		return true;
	//	}

	//	/**
	//	 * Gets result of intersection of this bounding-box with @a other bounding-box.
	//	 * In case the boxes don't intersect, the returned bounding-box is invalid.
	//	 * @param other Box to be tested
	//	 * @return Result of intersection.
	//	 * @see valid() method for more information on invalid bounding-boxes.
	//	 */
	//	template <typename SrcT>
	//	Aabb3<T> intersection(const Aabb3<SrcT>& other) const
	//	{
	//		Aabb3<T> ret;
	//		if (max.x < other.min.x || min.x > other.max.x)
	//			return ret;
	//		if (max.y < other.min.y || min.y > other.max.y)
	//			return ret;
	//		if (max.z < other.min.z || min.z > other.max.z)
	//			return ret;

	//		ret.min = std::max(min, other.min);
	//		ret.max = std::min(max, other.max);

	//		return ret;
	//	}

	//	/**
	//	 * Gets center point of bounding-box.
	//	 * @return A center point of bounding-box.
	//	 */
	//	Vector3<T> center() const
	//	{ return (min + max) * 0.5f; }

	//	/**
	//	 * Gets extent of bounding-box.
	//	 * @return Extent of bounding-box.
	//	 */
	//	Vector3<T> extent() const
	//	{ return (max - min) * 0.5f; }

	//	/**
	//	 * Gets diagonal size of bounding-box
	//	 * @return Sizes for particular dimensions.
	//	 */
	//	Vector3<T> size() const
	//	{ return max - min; }

	//	/**
	//	 * Gets all 8 corner-points of bounding box
	//	 * @param i An index of bounding-box corner point. Valid values are 0 .. 7.
	//	 * @return A position of @a i-th corner-point.
	//	 * @note The order of points is as follows (where @c + denotes max-point and @c - min-point):
	//	 * 1. (@c + @c + @c +)
	//	 * 2. (@c - @c + @c +)
	//	 * 3. (@c + @c - @c +)
	//	 * 4. (@c - @c - @c +)
	//	 * 5. (@c + @c + @c -)
	//	 * 6. (@c - @c + @c -)
	//	 * 7. (@c + @c - @c -)
	//	 * 8. (@c - @c - @c -)
	//	 *
	//	 */
	//	Vector3<T> point(size_t i) const
	//	{
	//		assert(i < 8);
	//		return Vector3<T>(i & 1 ? min.x : max.x, i & 2 ? min.y : max.y, i & 4 ? min.z : max.z);
	//	}

	//	/**
	//	 * Gets transformed bounding-box by transform @a t
	//	 * @param t A transform matrix
	//	 * @return Transformed bounding-box
	//	 */
	//	Aabb3<T> transformed(const Matrix4<T>& t) const
	//	{
	//		Aabb3<T> ret;
	//		for (size_t i = 0; i < 8; i++)
	//		{
	//			const Vector4<T> p(point(i), 1);
	//			ret.extend((t * p).xyz());
	//		}

	//		return ret;
	//	}

	//	//-------------------------------------------------------------------------------------------------------------
	//	// operators
	//	//-------------------------------------------------------------------------------------------------------------
	//	/**
	//	 * Tests if @a rhs is equal to this bounding-box
	//	 * @param rhs Right-hand side
	//	 * @return True if @a rhs and this bounding-boxes are equal, otherwise false
	//	 */
	//	template <typename RhsT>
	//	bool operator==(const Aabb3<RhsT>& rhs) const
	//	{
	//		return min == rhs.min && max == rhs.max;
	//	}

	//	/**
	//	 * Tests if @a rhs is not equal to this bounding-box
	//	 * @param rhs Right-hand side
	//	 * @return True if @a rhs and this bounding-boxes are not equal, otherwise false
	//	 */
	//	template <typename RhsT>
	//	bool operator!=(const Aabb3<RhsT>& rhs) const
	//	{
	//		return min != rhs.min || max != rhs.max;
	//	}

	//	/**
	//	 * Gets transformed bounding-box by transform @a rhs.
	//	 * @param rhs Matrix 4x4 representing the transform
	//	 * @return Transformed bounding-box
	//	 */
	//	Aabb3<T> operator*(const Matrix4<T>& rhs) const
	//	{
	//		return transformed(rhs);
	//	}

	//	/**
	//	 * Apply transform @a rhs to this bounding-box
	//	 * @param rhs A transform to be applied
	//	 * @return Reference to this
	//	 */
	//	Aabb3<T>& operator*=(const Matrix4<T>& rhs)
	//	{
	//		*this = transformed(rhs);
	//		return *this;
	//	}


	//	/**
	//	 * Extends this bounding-box by point @a rhs.
	//	 * @param rhs A point to extend this bounding-box by
	//	 * @return Reference to this
	//	 */
	//	template <typename SrcT>
	//	Aabb3<T>& operator<<(const Vector3<SrcT>& rhs)
	//	{
	//		extend(rhs);
	//		return *this;
	//	}

	//	/**
	//	 * Extends this bounding-box by box @a rhs.
	//	 * @param rhs A box to extend this bounding-box by
	//	 * @return Reference to this
	//	 */
	//	template <typename SrcT>
	//	Aabb3<T>& operator<<(const Aabb3<SrcT>& rhs)
	//	{
	//		extend(rhs);
	//		return *this;
	//	}

	//	/**
	//	 * Union of this and @a rhs bounding-boxes
	//	 * @param rhs Right-hand side of union
	//	 * @return A resulting bounding-box representing union
	//	 */
	//	template <typename RhsT>
	//	Aabb3<T> operator|(const Aabb3<RhsT>& rhs) const
	//	{
	//		return extended(rhs);
	//	}

	//	/**
	//	 * Intersection of this and @a rhs bounding-boxed
	//	 * @param rhs Right-hand side
	//	 * @return Resulting bouding-box representing the intersection.
	//	 */
	//	template <typename RhsT>
	//	Aabb3<T> operator&(const Aabb3<RhsT>& rhs) const
	//	{
	//		return intersection(rhs);
	//	}

	//	/**
	//	 * Outputs string representation of bounding-box @a rhs to output stream @a lhs
	//	 * @param lhs Output stream to write to
	//	 * @param rhs Bounding-box to write to output stream.
	//	 * @return Reference to output stream @a lhs
	//	 */
	//	friend std::ostream& operator<<(std::ostream& lhs, const Aabb3<T>& rhs)
	//	{
	//		lhs << rhs.min << " x " << rhs.max;
	//		return lhs;
	//	}

	//};

	//typedef Aabb3<float> Aabb3f;
	//typedef Aabb3<double> Aabb3d;

#ifdef VMATH_NAMESPACE
}
#endif //VMATH_NAMESPACE


#endif // __vmath_Header_File__

