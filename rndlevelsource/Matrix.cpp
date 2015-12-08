#include "stdafx.h"
#include "Matrix.h"
#include <math.h>
#include <assert.h>
#include <limits>
#include <sstream>
#include "Vertex.h"

#define M_PI 3.14159265358979323846

double Matrix::get(unsigned int x, unsigned int y) const
{
	if (x >= xsize || y >= ysize) return std::numeric_limits<double>::quiet_NaN();
	return arr[x][y];
}

void Matrix::set(unsigned int x, unsigned int y, double newval)
{
	if (x >= xsize || y >= ysize) return;
	arr[x][y] = newval;
}

unsigned int Matrix::x() const
{
	return xsize;
}

unsigned int Matrix::y() const
{
	return ysize;
}

unsigned int Matrix::rows() const
{
	return xsize;
}

unsigned int Matrix::cols() const
{
	return ysize;
}

void Matrix::copyfrom(const Matrix& orig)
{
	if (&orig == this) return;
	clear();
	xsize = orig.x();
	ysize = orig.y();
	arr = new double*[xsize];
	for (unsigned int n = 0; n < xsize; n++)
	{
		arr[n] = new double[ysize];
		for (unsigned int m = 0; m < ysize; m++)
		{
			arr[n][m] = orig.get(n, m);
		}
	}
}

void Matrix::setRow(unsigned int row, const Vertex& v)
{
	if (row >= x() || y() != 3) return;
	set(row, 0, v.x());
	set(row, 1, v.y());
	set(row, 2, v.z());
}

void Matrix::setCol(unsigned int col, const Vertex& v)
{
	if (col >= y() || x() != 3) return;
	set(0, col, v.x());
	set(1, col, v.y());
	set(2, col, v.z());
}

void Matrix::swap(Matrix& lhs, Matrix& rhs)
{
	std::swap(lhs.arr, rhs.arr);
	std::swap(lhs.xsize, rhs.xsize);
	std::swap(lhs.ysize, rhs.ysize);
}

Matrix Matrix::rotmatx(double deg)
{
	double rad = deg * (M_PI / 180.0);
	double sinth = sin(rad), costh = cos(rad);
	double rotarr[3][3] = {
		{1.0, 0.0, 0.0},
		{0.0, costh, -sinth},
		{0.0, sinth, costh}};
	return toMat(rotarr);
}

Matrix Matrix::rotmaty(double deg)
{
	double rad = deg * (M_PI / 180.0);
	double sinth = sin(rad), costh = cos(rad);
	double rotarr[3][3] = {
		{costh, 0.0, sinth},
		{0.0, 1.0, 0.0},
		{-sinth, 0.0, costh}};
	return toMat(rotarr);
}

Matrix Matrix::rotmatz(double deg)
{
	double rad = deg * (M_PI / 180.0);
	double sinth = sin(rad), costh = cos(rad);
	double rotarr[3][3] = {
		{costh, -sinth, 0.0},
		{sinth, costh, 0.0},
		{0.0, 0.0, 1.0}};
	return toMat(rotarr);
}

Matrix Matrix::rotmatxPassive(double deg)
{
	double rad = deg * (M_PI / 180.0);
	double sinth = sin(rad), costh = cos(rad);
	double rotarr[3][3] = {
		{1.0, 0.0, 0.0},
		{0.0, costh, sinth},
		{0.0, -sinth, costh}};
	return toMat(rotarr);
}

Matrix Matrix::rotmatyPassive(double deg)
{
	double rad = deg * (M_PI / 180.0);
	double sinth = sin(rad), costh = cos(rad);
	double rotarr[3][3] = {
		{costh, 0.0, -sinth},
		{0.0, 1.0, 0.0},
		{sinth, 0.0, costh}};
	return toMat(rotarr);
}

Matrix Matrix::rotmatzPassive(double deg)
{
	double rad = deg * (M_PI / 180.0);
	double sinth = sin(rad), costh = cos(rad);
	double rotarr[3][3] = {
		{costh, sinth, 0.0},
		{-sinth, costh, 0.0},
		{0.0, 0.0, 1.0}};
	return toMat(rotarr);
}

Matrix& Matrix::operator=(const Matrix& rhs)
{
	this->copyfrom(rhs);
	return *this;
}

Matrix& Matrix::operator=(Matrix&& rhs)
{
	Matrix::swap(*this, rhs);
	return *this;
}

Matrix& Matrix::operator*=(const Matrix& rhs)
{
	//mictimer multtimer("ms operator*=()", 1000.0);
	Matrix prod = this->mult(rhs);
	copyfrom(prod);
	return *this;
}

Matrix& Matrix::operator*=(double rhs)
{
	for (unsigned int n = 0; n < x(); n++) for (unsigned int m = 0; m < y(); m++) arr[n][m] *= rhs;
	return *this;
}

inline double dotmult(const Matrix& rowmat, const Matrix& colmat, unsigned int row, unsigned int col)
{
	if (rowmat.y() != colmat.x()) return std::numeric_limits<double>::quiet_NaN();
	double res = 0.0;
	for (unsigned int n = 0; n < rowmat.y(); n++)
	{
		res += (rowmat.get(row, n) * colmat.get(n, col));
	}
	return res;
}

Matrix Matrix::mult(const Matrix& rhs)
{
	if (y() != rhs.x()) return Matrix(0, 0);
	Matrix ret(x(), rhs.y());
	for (unsigned int n = 0; n < x(); n++)
	{
		for (unsigned int m = 0; m < rhs.y(); m++)
		{
			ret.set(n, m, dotmult(*this, rhs, n, m));
		}
	}
	return ret;
}

std::string Matrix::toStr() const
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

void Matrix::clear()
{
	if (arr == nullptr)
	{
		xsize = 0;
		ysize = 0;
		return;
	}
	for (unsigned int n = 0; n < xsize; n++)
	{
		if (arr[n] != nullptr) delete [] arr[n];
	}
	if (arr != nullptr) delete [] arr;
	arr = nullptr;
	xsize = 0;
	ysize = 0;
}

void Matrix::printpretty() const
{
	for (unsigned int n = 0; n < x(); n++)
	{
		for (unsigned int m = 0; m < y(); m++)
		{
			printf("%6.3f ", get(n, m));
		}
		printf("\n");
	}
}

inline void rowSet(Matrix& mat, const Matrix& row, unsigned int to)
{
	for (unsigned int n = 0; n < mat.cols(); n++)
	{
		mat[to][n] = row[0][n];
	}
}

inline void colSet(Matrix& mat, const Matrix& col, unsigned int to)
{
	for (unsigned int n = 0; n < mat.rows(); n++)
	{
		mat[n][to] = col[n][0];
	}
}

inline Matrix rowGet(const Matrix& mat, unsigned int from, double mod = 0.0)
{
	Matrix rowcpy(1, mat.cols());
	for (unsigned int n = 0; n < mat.cols(); n++) rowcpy[0][n] = mat[from][n];
	rowcpy *= mod;
	return rowcpy;
}

inline void rowAdd(Matrix& mat, const Matrix& row, unsigned int to)
{
	for (unsigned int n = 0; n < mat.y(); n++)
	{
		mat[to][n] += row[0][n];
	}
}

inline void rowSwap(Matrix& mat, unsigned int r1, unsigned int r2)
{
	Matrix row1 = rowGet(mat, r1);
	rowSet(mat, rowGet(mat, r2), r1);
	rowSet(mat, row1, r2);
}

inline void rowMod(Matrix& mat, unsigned int row, double mod)
{
	for (unsigned int col = 0; col < mat.cols(); col++) mat[row][col] *= mod;
}

inline unsigned int rowPivot(Matrix& mat, unsigned int currentrow)
{
	unsigned int workingrow = currentrow;
	while (doubleeq(mat[currentrow][workingrow], 0.0))
	{
		if (workingrow == mat.rows() - 1) return 0xFFFFFFFF;
		rowSwap(mat, currentrow, ++workingrow);
	}
	return workingrow;
}

inline Matrix rowGetSkip(const Matrix& mat, unsigned int from, unsigned int skip)
{
	Matrix rowcpy(1, mat.cols() - 1);
	unsigned int putcnt = 0;
	for (unsigned int n = 0; n < mat.cols(); n++)
	{
		if (n == skip) continue;
		rowcpy[0][putcnt++] = mat[from][n];
	}
	return rowcpy;
}

inline Matrix colGetSkip(const Matrix& mat, unsigned int from, unsigned int skip)
{
	Matrix rowcpy(mat.rows() - 1, 1);
	unsigned int putcnt = 0;
	for (unsigned int n = 0; n < mat.rows(); n++)
	{
		if (n == skip) continue;
		rowcpy[putcnt++][0] = mat[n][from];
	}
	return rowcpy;
}

// Probably very slow
double Matrix::det() const
{
	if (cols() != rows()) return std::numeric_limits<double>::quiet_NaN();
	if (cols() == 2) return (arr[0][0] * arr[1][1]) - (arr[0][1] * arr[1][0]);
	double ret = 0.0;
	int sign = 1;
	for (unsigned int n = 0; n < cols(); n++)
	{
		Matrix newmat(cols() - 1, rows() - 1);
		double mod = arr[0][n] * sign;
		sign *= -1;
		unsigned int colput = 0;
		for (unsigned int m = 0; m < cols(); m++)
		{
			if (m == n) continue;
			Matrix col = colGetSkip((*this), m, 0);
			colSet(newmat, col, colput++);
		}
		ret += (mod * newmat.det());
	}
	return ret;
}

Matrix Matrix::gaussElim(Matrix mat)
{
	if (mat.x() + 1 != mat.y()) return Matrix(0, 0);
	for (unsigned int row = 0; row < mat.rows(); row++)
	{
		unsigned int workingrow = rowPivot(mat, row);
		if (workingrow == 0xFFFFFFFF)
		{
			return Matrix(0, 0);
		}
		rowMod(mat, workingrow, 1.0 / mat[workingrow][workingrow]);
		for (unsigned int modrow = workingrow + 1; modrow < mat.rows(); modrow++)
		{
			double mod = mat[workingrow][workingrow] * mat[modrow][workingrow];
			rowAdd(mat, rowGet(mat, workingrow, -mod), modrow);
		}
	}
	for (int row = mat.rows() - 1; row >= 0; row--)
	{
		double varval = mat[row][mat.cols() - 1];
		for (int modrow = row - 1; modrow >= 0; modrow--)
		{
			mat[modrow][mat.cols() - 1] -= varval * mat[modrow][row];
			mat[modrow][row] = 0.0;
		}
	}
	Matrix ret(mat.rows(), 1);
	for (unsigned int row = 0; row < mat.rows(); row++)
	{
		ret[row][0] = mat[row][mat.cols() - 1];
	}
	return ret;
}

Matrix::Matrix(double** matarr, unsigned int xs, unsigned int ys) :
	xsize(xs),
	ysize(ys),
	arr(matarr)
{
}

Matrix::Matrix(const Matrix& orig) :
	xsize(0),
	ysize(0),
	arr(nullptr)
{
	this->copyfrom(orig);
}

Matrix::Matrix(Matrix&& orig) :
	xsize(0),
	ysize(0),
	arr(nullptr)
{
	Matrix::swap(*this, orig);
}

Matrix::Matrix(unsigned int xs, unsigned int ys, double sval) :
	xsize(xs),
	ysize(ys),
	arr(nullptr)
{
	if (xs == 0 || ys == 0) return;
	arr = new double*[xs];
	for (unsigned int n = 0; n < xs; n++)
	{
		arr[n] = new double[ys];
		for (unsigned int m = 0; m < ys; m++)
		{
			arr[n][m] = sval;
		}
	}
}

Matrix::~Matrix(void)
{
	clear();
}

