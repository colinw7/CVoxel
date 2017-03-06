#include <CVoxel.h>
#include <iostream>

int
main(int argc, char **argv)
{
  std::string filename;
  bool        raytrace = false;
  bool        debug = false;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      std::string opt = std::string(argv[i]).substr(1);

      if      (opt == "raytrace")
        raytrace = true;
      else if (opt == "debug")
        debug = true;
      else
        std::cerr << "Invalid option '" << opt << "'" << std::endl;
    }
    else {
      filename = argv[i];
    }
  }

  CVoxel vox;

  vox.setDebug(debug);

  if (filename != "") {
    if (! vox.readVox(filename))
      exit(1);
  }

  if (raytrace)
    vox.outputRaytrace();

  return 0;
}
