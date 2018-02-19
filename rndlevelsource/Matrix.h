#pragma once

#include <array>
#include <sstream>
#include <string>
#include <initializer_list>
#include "utils.h"

#define M_PI 3.14159265358979323846

template <class _Ty, size_t _X, size_t _Y>
class Matrix
{
private:
	std::array<_Ty, _X * _Y> arr_;

public:

	static constexpr size_t size = _X * _Y;

	class matrow
	{
		size_t row;
		Matrix<_Ty, _X, _Y>& parent;

	public:
		matrow(size_t r, Matrix<_Ty, _X, _Y>& p) : row(r), parent(p)
		{
		}

		_Ty& operator[](size_t col) const
		{
			return parent.arr_[row + col * parent.x()];
		}
	};

	class const_matrow
	{
		size_t row;
		const Matrix<_Ty, _X, _Y>& parent;

	public:
		const_matrow(size_t r, const Matrix<_Ty, _X, _Y>& p) : row(r), parent(p)
		{
		}

		_Ty operator[](size_t col) const
		{
			return parent.arr_[row + col * parent.x()];
		}
	};

	matrow operator[](size_t row)
	{
		return matrow(row, *this);
	}

	const_matrow operator[](size_t row) const
	{
		return const_matrow(row, *this);
	}

	constexpr size_t x() const { return _X; }
	constexpr size_t y() const { return _Y; }
	constexpr size_t rows() const { return _X; }
	constexpr size_t cols() const { return _Y; }

	_Ty get(size_t x, size_t y) const
	{
		if (x >= this->x() || y >= this->y())
			throw std::exception("Out of range");

		return arr_[x + y * this->x()];
	}

	void set(size_t x, size_t y, _Ty value)
	{
		if (x >= this->x() || y >= this->y())
			throw std::exception("Out of range");

		arr_[x + y * this->x()] = value;
	}

	void setRow(size_t row, const Matrix<_Ty, _X, 1>& v)
	{
		for (size_t col = 0; col < cols(); col++)
			set(row, col, v.x());
	}

	void setCol(size_t col, const Matrix<_Ty, _X, 1>& v)
	{
		for (size_t row = 0; row < rows(); row++)
			set(row, col, v.x());
	}

	static void swap(Matrix<_Ty, _X, _Y>& lhs, Matrix<_Ty, _X, _Y>& rhs)
	{
		std::swap(lhs.arr_, rhs.arr_);
	}

	//Returns a matrix that will transform a column vector by deg degrees around the x-axis.
	static Matrix<_Ty, 3, 3> rotmatx(_Ty deg)
	{
		static_assert(_X == 3 && _Y == 3, "Only available with 3x3 matrix");

		_Ty rad = deg * (M_PI / 180.0);
		_Ty sinth = sin(rad), costh = cos(rad);
		Matrix<_Ty, 3, 3> mat =
		{
			 1,	0, 0,
			 0,	costh, -sinth,
			 0,	sinth, costh
		};

		return mat;
	}

	//Returns a matrix that will transform a column vector by deg degrees around the y-axis.
	static Matrix<_Ty, 3, 3> rotmaty(_Ty deg)
	{
		static_assert(_X == 3 && _Y == 3, "Only available with 3x3 matrix");

		_Ty rad = deg * (M_PI / 180.0);
		_Ty sinth = sin(rad), costh = cos(rad);
		Matrix<_Ty, 3, 3> mat =
		{
			costh, 0, sinth,
			0, 1, 0,
			-sinth,	0, costh
		};
		return mat;
	}

	//Returns a matrix that will transform a column vector by deg degrees around the z-axis.
	static Matrix<_Ty, 3, 3> rotmatz(_Ty deg)
	{
		static_assert(_X == 3 && _Y == 3, "Only available with 3x3 matrix");

		_Ty rad = deg * (M_PI / 180.0);
		_Ty sinth = sin(rad), costh = cos(rad);
		Matrix<_Ty, 3, 3> mat =
		{
			costh, -sinth, 0,
			sinth, costh, 0,
			0, 0, 1
		};
		return mat;
	}

	////Returns a matrix that will transform a row vector by deg degrees around the x-axis.
	//static Matrix<_Ty, _X, _Y> rotmatxPassive(double deg);
	////Returns a matrix that will transform a row vector by deg degrees around the y-axis.
	//static Matrix<_Ty, _X, _Y> rotmatyPassive(double deg);
	////Returns a matrix that will transform a row vector by deg degrees around the z-axis.
	//static Matrix<_Ty, _X, _Y> rotmatzPassive(double deg);

	//Returns a string representing the matrix compliant with wolframalpha's syntax.
	std::string toStr() const
	{
		std::ostringstream os;
		os << "{";
		for (unsigned int n = 0; n < x(); n++)
		{
			os << "{";
			for (unsigned int m = 0; m < y(); m++)
			{
				if (m != 0) os << ", ";
				os << get(n, m);
			}
			os << "}";
			if (n != x() - 1) os << ",";
		}
		os << "}";
		return os.str();
	}

	Matrix<_Ty, _X, _Y>& operator=(const Matrix& rhs)
	{
		std::copy(rhs.arr_.cbegin(), rhs.arr_.cend(), arr_.begin());
		return *this;
	}

	Matrix<_Ty, _X, _Y>& operator*=(double mult)
	{
		for (auto &v : arr_)
			v *= mult;

		return *this;
	}

	bool operator==(const Matrix &rhs) const
	{
		if (rhs.size != size)
			return false;

		for (size_t n = 0; n < size; n++)
			if (!doubleeq(rhs.arr_[n], arr_[n]))
				return false;

		return true;
	}

	//Returns the product of the matrix(lhs) multiplied by the argument matrix(rhs).
	template <size_t _X2, size_t _Y2>
	Matrix<_Ty, _X, _Y2> mult(const Matrix<_Ty, _X2, _Y2>& rhs) const
	{
		static_assert(_Y == _X2, "Invalid matrix multiplication");

		Matrix<_Ty, _X, _Y2> ret;
		for (size_t n = 0; n < x(); n++)
		{
			for (size_t m = 0; m < rhs.y(); m++)
			{
				_Ty res = 0.0;
				for (size_t i = 0; i < y(); i++)
					res += (get(n, i) * rhs.get(i, m));

				ret.set(n, m, res);
			}
		}
		return ret;
	}

	Matrix(_Ty initVal = 0)
	{
		for (auto &v : arr_)
			v = initVal;
	}

	Matrix(const Matrix<_Ty, _X, _Y>& mat)
	{
		for (size_t n = 0; n < size; n++)
			arr_[n] = mat.arr_[n];
	}

	Matrix(std::initializer_list<_Ty> list)
	{
		size_t cnt = 0;
		for (auto &val : list)
		{
			if (cnt == size)
				break;
			arr_[cnt++] = val;
		}
	}
};

typedef Matrix<double, 3, 3> Matrix3d;

template <class _Ty, size_t _X, size_t _Y>
std::ostream& operator<<(std::ostream& stream, const Matrix<_Ty, _X, _Y>& matrix)
{
	stream << matrix.toStr();
	return stream;
}

template <class _Ty, size_t _X1, size_t _Y1, size_t _X2, size_t _Y2>
Matrix<_Ty, _X1, _Y2> operator*(const Matrix<_Ty, _X1, _Y1>& lhs, const Matrix<_Ty, _X2, _Y2>& rhs)
{
	return lhs.mult(rhs);
}

template <class _Ty, size_t _X, size_t _Y>
Matrix<_Ty, _X, _Y> operator*(Matrix<_Ty, _X, _Y> lhs, _Ty rhs)
{
	lhs *= rhs;
	return lhs;
}
