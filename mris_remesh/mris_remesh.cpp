#include "argparse.h"
#include "mrisurf.h"
#include "remesher.h"
#include "surfgrad.h"
#include "mris_remesh.help.xml.h"


int main(int argc, const char **argv) 
{
  ArgumentParser parser;
  parser.addHelp(mris_remesh_help_xml, mris_remesh_help_xml_len);
  // required
  parser.addArgument("-i", "--input", 1, String, true);
  parser.addArgument("-o", "--output", 1, String, true);
  // one of these is required
  parser.addArgument("--nvert", 1, Int);
  parser.addArgument("--edge-len", 1, Float);
  // optional
  parser.addArgument("--iters", 1, Int);
  parser.parse(argc, argv);

  // read input surface
  std::string inputname = parser.retrieve<std::string>("input");
  MRIS *surf = MRISread(inputname.c_str());
  if (!surf) fs::fatal() << "could not read input surface " << inputname;

  // number of iterations
  int iters = parser.exists("iters") ? parser.retrieve<int>("iters") : 5;

  // init the remesher
  Remesher remesher = Remesher(surf);

  // remesh with specified method
  if (parser.exists("nvert")) {

    // quick sanity check to make sure edge len wasn't set as well
    if (parser.exists("edge-len")) fs::fatal() << "specify target edge len OR target num vertices, not both";

    // remesh to number of vertices
    int nverts = parser.retrieve<int>("nvert");
    remesher.remeshBKV(iters, nverts, false);
  }
  else if (parser.exists("edge-len")) {
    // remesh to target edge length
    float length = parser.retrieve<float>("edge-len");
    remesher.remeshBK(iters, length);
  }
  else {
    fs::fatal() << "must specify target edge length or number of vertices";
  }

  // convert back to MRIS
  MRIS *remeshed = remesher.toSurface();
  MRIScopyMetadata(surf, remeshed);

  // remove the sea of intersections
  std::cout << "Removing intersections" << std::endl;
  MRISremoveIntersections(remeshed);

  // print stats
  double diff = (double)remeshed->nvertices / (double)surf->nvertices;
  printf("Remeshed surface quality stats nv0 = %d  nv = %d  %g\n", surf->nvertices, remeshed->nvertices, diff);
  MRISedges(remeshed);
  MRIScorners(remeshed);
  MRISfaceMetric(remeshed, 0);
  MRISedgeMetric(remeshed, 0);
  MRIScornerMetric(remeshed, 0);
  MRISprettyPrintSurfQualityStats(stdout, remeshed);

  // save
  std::string outputname = parser.retrieve<std::string>("output");
  MRISwrite(remeshed, outputname.c_str());

  MRISfree(&surf);
  MRISfree(&remeshed);

  exit(0);
}
