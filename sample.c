#include <stdio.h>

#include <stdlib.h>



struct MJArray {

  void *begin, *end;

  int size;

};



int ord(char c) {

  return (int) c;

}



char chr(int i) {

  return (char) i;

}



int len(struct MJArray arr) {

  return (arr.end - arr.begin) / arr.size;

}



void mj_main() {

  struct MJArray arr;

  int a[5];

  puts("a: "+ord('a'));

  puts("90: "+chr(90));

  arr.begin = a;

  arr.end = a + 4;

  arr.size = sizeof(int);

  puts("size: "+len(arr));

}



int main(int argc, char **argv) {

  mj_main();

  return 0;

}


