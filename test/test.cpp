
#include <iostream>
#include <string>
#include "llvmjConfig.h"
#include "llvm/ADT/StringMap.h"

using namespace std;

class A {
	private:
	int x;
	
	public:
	A() { x = 0; }
	A(int i) { x = i; }
	A(const A &a) { x = a.x; }
	int X() const { return x; }
	
};

ostream& operator<<(ostream &os, const A &a) {
	return os << a.X();
}

int main(int argc, char **argv) {
	llvm::StringMap<A> map;
	A a = A(1);
	map["jedan"] = a;
	map["jedan"] = A(2);
	
	A br = 3;
	
	cout << "Hello World!" << endl;
	
	A jedan = map["jedan"];
	
	cout << jedan << endl;
	cout << a << endl;
	cout << map["nema"] << endl;
} 

