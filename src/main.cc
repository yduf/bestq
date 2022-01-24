/* compile with:
 *      g++ -g -Wall example.cc `pkg-config vips-cpp --cflags --libs`
 */

#include <cstdint>
#include <iostream>

#include <vips/vips8>
#include <nlohmann/json.hpp>


// for convenience
using json = nlohmann::json;
using namespace vips;
using namespace std;

constexpr bool debug = false;


int main( int argc, char** argv) {
  if (VIPS_INIT (argv[0])) 
    vips_error_exit (NULL);

  if (argc < 1+1)
    vips_error_exit ("usage: %s file* ", argv[0]);

  int i = 1;
  for( const char* name=argv[i++];i<=argc;name=argv[i++]) {
      cerr << name << "\n";
  }

  vips_shutdown ();


  return 0;
}