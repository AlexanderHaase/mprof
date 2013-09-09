#include <stdlib.h>

int main( void ) {
  for( int i = 0; i < 1000; i++ ) {
    int * ptr = malloc( sizeof( int ) );
    *ptr = i;
    free( ptr );
  }

  return 0;
}
