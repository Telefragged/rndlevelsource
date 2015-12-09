#pragma once

#include <ios>
#include <string>

class Vertex;

class Matrix
{
private:
	unsigned int xsize_, ysize_;
	double* arr_;

public:
	class matrow
	{
		unsigned int row;
		Matrix& parent;
	public:
		matrow(unsigned int r, Matrix& p) : row(r), parent(p)
		{
		}

		double& operator[](unsigned int col) const
		{
			return parent.arr_[row + col * parent.xsize_];
		}
	};

	class const_matrow
	{
		unsigned int row;
		const Matrix& parent;
	public:
		const_matrow(unsigned int r, const Matrix& p) : row(r), parent(p)
		{
		}

		double operator[](unsigned int col) const
		{
			return parent.arr_[row + col * parent.xsize_];
		}
	};

	matrow operator[](unsigned int row)
	{
		return matrow(row, *this);
	}

	const_matrow operator[](unsigned int row) const
	{
		return const_matrow(row, *this);
	}

	unsigned int x() const;
	unsigned int y() const;
	unsigned int rows() const;
	unsigned int cols() const;

	double get(unsigned int, unsigned int) const;
	void set(unsigned int, unsigned int, double);
	void setRow(unsigned int, const Vertex&);
	void setCol(unsigned int, const Vertex&);

	void clear();
	void copyfrom(const Matrix&);
	void printpretty() const;

	static void swap(Matrix&, Matrix&);

	//Returns a matrix that will transform a collumn vector by deg degrees around the x-axis.
	static Matrix rotmatx(double deg);
	//Returns a matrix that will transform a collumn vector by deg degrees around the y-axis.
	static Matrix rotmaty(double deg);
	//Returns a matrix that will transform a collumn vector by deg degrees around the z-axis.
	static Matrix rotmatz(double deg);

	//Returns a matrix that will transform a row vector by deg degrees around the x-axis.
	static Matrix rotmatxPassive(double deg);
	//Returns a matrix that will transform a row vector by deg degrees around the y-axis.
	static Matrix rotmatyPassive(double deg);
	//Returns a matrix that will transform a row vector by deg degrees around the z-axis.
	static Matrix rotmatzPassive(double deg);

	//Returns a string representing the matrix compliant with wolframalpha's syntax.
	std::string toStr() const;

	Matrix& operator=(const Matrix&);
	Matrix& operator=(Matrix&&);

	Matrix& operator*=(const Matrix&);
	Matrix& operator*=(double);

	double det() const;

	//Returns the product of the matrix(lhs) multiplied by the argument matrix(rhs).
	Matrix mult(const Matrix&);

	static Matrix gaussElim(Matrix mat);

	Matrix(unsigned int, unsigned int, double sval = 0.0);
	Matrix(const Matrix&);
	Matrix(Matrix&&);
	~Matrix(void);
};

inline std::ostream& operator<<(std::ostream& stream, const Matrix& Matrix)
{
	stream << Matrix.toStr();
	return stream;
}

inline Matrix operator*(Matrix lhs, const Matrix& rhs)
{
	lhs *= rhs;
	return lhs;
}

inline Matrix operator*(Matrix lhs, double rhs)
{
	lhs *= rhs;
	return lhs;
}

template <unsigned int X, unsigned int Y>
inline Matrix toMat(double (&arr)[X][Y])
{
	Matrix ret(X, Y);
	for (unsigned int n = 0; n < X; n++)
	{
		for (unsigned int m = 0; m < Y; m++)
		{
			ret[n][m] = arr[n][m];
		}
	}
	return ret;
}

