#pragma once

#include <string>

class Matrix;

//For all intents and purposes a vector from the
//coordinate systems origin.

class Vertex {
protected:
    double vertex_[3];
public:
    virtual std::string toStr() const;

    static bool isVertex(const Vertex &);

    static Vertex allmin(const Vertex &, const Vertex &);
    static Vertex allmax(const Vertex &, const Vertex &);

    void parsestr(const std::string &);

    Vertex rotate(const Matrix&);
    Vertex rotate(const Vertex&, const Matrix&);

    void rotateInPlace(const Matrix&);
    void rotateInPlace(const Vertex &, const Matrix &);
	
    virtual void x(double x);
    virtual void y(double y);
    virtual void z(double z);

    virtual double x() const;
    virtual double y() const;
    virtual double z() const;

    Matrix toMat();

    double length() const;
    double dotProduct(const Vertex &);

    Vertex normalize();
    Vertex crossProduct(const Vertex &);

    static bool parallel(const Vertex &, const Vertex &);
    static bool equals(const Vertex &, const Vertex &);

    static double length(const Vertex &);
    static double dotProduct(const Vertex &, const Vertex &);
	static double dotProduct2D(const Vertex &, const Vertex &);

    static Vertex normalize(const Vertex &);
    static Vertex crossProduct(const Vertex &, const Vertex &);

    Vertex &operator=(const Vertex &);
    Vertex &operator+=(const Vertex &);
    Vertex &operator-=(const Vertex &);
    Vertex &operator*=(double);

    double operator[](unsigned int) const;
    double &operator[](unsigned int);

    Vertex(void);
    Vertex(const std::string &);
    Vertex(double, double, double);
    virtual ~Vertex(void);
};

inline Vertex operator+(Vertex lhs, const Vertex &rhs) {
    lhs += rhs;
    return lhs;
}

inline Vertex operator-(Vertex lhs, const Vertex &rhs) {
    lhs -= rhs;
    return lhs;
}

inline Vertex operator*(Vertex lhs, double rhs) {
    lhs *= rhs;
    return lhs;
}