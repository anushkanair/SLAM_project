#include <iostream>
#include <string>
#include <stdexcept>

// have a class representing a complex number
//functions with operators (+. *, /, -)

using namespace std; //declares the namespace scope for identifiers in the code

class complexNumber { //creates a class representing a complex number (creates a type)
    public :
        double real;
        double imag;

    void printComplexNumber(); // forward declaration of function so they can be in the scope of the class
    complexNumber add(complexNumber B);
    complexNumber multiply(complexNumber B);
    complexNumber divide(complexNumber B);
};

complexNumber complexNumber::add(complexNumber B)//when forward declared in code to show scope use <return type> <class name>::<fxn name>(input)
{
    complexNumber C; // declares a number with type complex Number
    C.imag = imag + B.imag; //adds imaginary parts of the number
    C.real = real + B.real; //adds the real parts of the number
    return C; //returns the new number
}

complexNumber subtract(complexNumber A, complexNumber B)
{
    complexNumber C;
    C.imag = A.imag - B.imag;
    C.real = A.real - B.real;
    return C;
}

complexNumber complexNumber::multiply(complexNumber B) {
    complexNumber C;
    C.real = (real*B.real - imag*B.imag);
    C.imag = (real*B.imag + imag*B.real);
    return C;
}

complexNumber complexNumber::divide(complexNumber B)
{
    complexNumber C;
    complexNumber F;
    double G;
    F.imag = -B.imag;
    F.real = B.real;
    G = (B.real*B.real + B.imag*B.imag);
    C.real = (real*F.real - imag*F.imag)/ G;
    C.imag = (real*F.imag + imag*F.real)/G;
    if (G==0) {
        throw (runtime_error("This is not a valid divisor"));
    }
    return C;
}

void complexNumber::printComplexNumber() { //function that prints the new complex number
    if (real < 0) { //if the real part of the new number is less than 0
    cout << imag << "i - " << -real << endl; //print it in this certain format
    }
    else { //if not
    cout << imag << "i + " << real << endl; //print it in this format
    }
}

int main()
{
    complexNumber d; //declares two complex numbers
    d.imag = 5;
    d.real = 3;
    complexNumber e;
    e.imag = 0;
    e.real = 0;

    d.add(e).printComplexNumber(); //calls functions using those two numbers
    d.multiply(e).printComplexNumber();
    d.divide(e).printComplexNumber();
    return 0;
}
