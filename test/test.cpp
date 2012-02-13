
#include <iostream>
#include <string>
#include "llvmjConfig.h"
#include "llvm/ADT/StringMap.h"

using namespace std;

class A {
private:
	const int x;
    int *y;
	
public:
	A() : x(-1) { }
	A(int i) : x(i) { }
	A(const A &a) : x(a.x) { }
	int X() const { return x; }
    void setY(int &_y) { y = &_y; }
    int & Y() {return }
	
};

ostream& operator<<(ostream &os, const A &a) {
	return os << a.X();
}

int main(int argc, char **argv) {
	llvm::StringMap<const A*> map;
	const A a = A(1);
	const A b = A(2);
	map["jedan"] = &a;
	map["jedan"] = &b;
	
	A br = 3;
	
	cout << "Hello World!" << endl;
	
	A jedan = * map["jedan"];
	
	cout << jedan << endl;
	cout << a << endl;
	cout << map["nema"] << endl;
} 

