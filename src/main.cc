/* compile with:
 *      g++ -g -Wall example.cc `pkg-config vips-cpp --cflags --libs`
 */

#include <cstdint>
#include <iostream>
#include <map>
//#include <ranges>

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

   int akernel[] = { 1,-2, 1,
                    -2, 4,-2,
                     1,-2, 1 };

   VImage laplacian = VImage::new_from_memory( akernel, sizeof(akernel), 
                                            3, 3, 1, VIPS_FORMAT_INT);

  // 1 - gather max dimension from images
  // we don't care about form factors
  int i = 1;
  int max_width = 0;
  int max_height = 0;

  map<string, double> noise;
  double min_sigma = INFINITY;

  for( const char* name=argv[i++];i<=argc;name=argv[i++]) {
    cerr << name << "\n";
    VImage in = VImage::new_from_file (name, VImage::option ()->set ("access", VIPS_ACCESS_SEQUENTIAL) );
    
    if( debug)
      cerr << "  " << in.width() << "x" << in.height() << "\n";

    if (in.width() > max_width)
      max_width = in.width();

    if( in.height() > max_height)
      max_height = in.height();

    // estimate noise
    VImage conv = in.colourspace(VIPS_INTERPRETATION_sRGB, 
                                 VImage::option()
                                    ->set ("source_space", VIPS_INTERPRETATION_B_W)
                                  )
                      .conv( laplacian )
                      .abs()
                ;

    if( debug)
      conv.write_to_file("conv.pgm");
    
    VImage sum_rows; // = VImage::VImage::new_memory();
    VImage sum_columns = conv.project( &sum_rows) / ( 6.0 * (in.height() -2) );
    // cerr << "sum_rows  " << sum_rows.width() << "x" << sum_rows.height() << "\n";
    // cerr << "sum_columns  " << sum_columns.width() << "x" << sum_columns.height() << "\n";
             
    VImage sum = VImage::VImage::new_memory();
    sum_columns.project( &sum );
    // cerr << "sum  " << sum.width() << "x" << sum.height() << "\n";

    auto* p = (double*) sum[0].data();
    double sigma = sqrt( 0.5 * M_PI) / ( conv.width()-2)* *p;
    // cerr << "sigma  " << sigma << "\n";

    noise[name] = sigma;

    if (sigma < min_sigma)
      min_sigma = sigma;

    /*
    auto sigma = sqrt( 0.5 * M_PI) / (6 * ( in.width()-2) * (in.height()-2))* *p;
    cerr << "  sigma: " << sigma << "\n";

    */
  }

  if( debug) {
    cerr << "max_width: " << max_width << "\n";
    cerr << "max_height: " << max_height << "\n";
  }

  // 2 - rescale all images to max dimension
  // 3 - save them in jpg with same parameters
  // and use the size as an indicator of the sharpness.
  std::multimap<size_t, string> sharpness_map;

  i = 1;
  for( const char* name=argv[i++];i<=argc;name=argv[i++]) {
    if( debug)
      cerr << name << "\n";

    void *buf;
    size_t size;

      VImage rescaled = VImage::thumbnail( name, max_width,
                                      VImage::option()
                                        ->set( "size", VIPS_SIZE_FORCE)
                                        ->set( "height", max_height)
                                        ->set( "no_rotate", true)
                                    )
                        .colourspace(VIPS_INTERPRETATION_sRGB, 
                                 VImage::option()
                                    ->set ("source_space", VIPS_INTERPRETATION_B_W)
                                  )
                        ;          
        rescaled.write_to_buffer( ".jpg", &buf, &size );
        // cerr << size << "\n";

        //sharpness_map[size] = name;
        sharpness_map.insert(  std::multimap<size_t, string>::value_type( size, name));

        free( buf);
  }

  // output json
      // JSON output
    json j;
    int max_sharpness = sharpness_map.rbegin()->first;

    for( const auto& [size, file] : sharpness_map) {
        /*j["sharpness"].push_back({
            {"size", k},
            {"file", v}
        });*/
        json e = {  { "file",  file},
                    { "sharpness", (double) size / max_sharpness},  // 1.0 is max sharpness (normalized)
                    { "jpg_size", size},                            // used to evaluate absolute sharpness
                    { "noise", noise[file] / min_sigma },           // 1.0 is min noise (normalized)
                    { "sigma_noise", noise[file] }                  // absolute noise
                };
        j.push_back( e);
//        cerr << k << " " << v << "\n" << endl;
    }

    std::cout << j << "\n";

  vips_shutdown ();


  return 0;
}