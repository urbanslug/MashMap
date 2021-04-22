/**
 * @file    mash_map.cpp
 * @ingroup src
 * @author  Chirag Jain <cjain7@gatech.edu>
 */

#include <iostream>
#include <ctime>
#include <chrono>
#include <functional>

//Own includes
#include "map/include/map_parameters.hpp"
#include "map/include/base_types.hpp"
#include "map/include/parseCmdArgs.hpp"
#include "map/include/winSketch.hpp"
#include "map/include/computeMap.hpp"

//External includes
#include "common/argvparser.hpp"
#include "common/ALeS.hpp"

int main(int argc, char** argv)
{
  /*
   * Make sure env variable MALLOC_ARENA_MAX is unset 
   * for efficient multi-thread execution
   */
  unsetenv((char *)"MALLOC_ARENA_MAX");

  CommandLineProcessing::ArgvParser cmd;

  //Setup command line options
  skch::initCmdParser(cmd);

  //Parse command line arguements   
  skch::Parameters parameters;        //sketching and mapping parameters

  skch::parseandSave(argc, argv, cmd, parameters);   

  auto t0 = skch::Time::now();

  std::cerr << "Generating spaced seeds" << std::endl;
  uint32_t seed_weight = 10;
  uint32_t seed_count = 5;
  float similarity = 0.75;
  uint32_t region_length = 20;

  std::vector<ales::spaced_seed> spaced_seeds = ales::generate_spaced_seeds(seed_weight, seed_count, similarity, region_length);
  std::chrono::duration<double> time_spaced_seeds = skch::Time::now() - t0;
  std::cout << "INFO, Time spent generating spaced seeds " << time_spaced_seeds.count()  << " seconds" << std::endl;

  //Build the sketch for reference
  skch::Sketch referSketch(parameters, spaced_seeds);

  std::chrono::duration<double> timeRefSketch = skch::Time::now() - t0;
  std::cout << "INFO, skch::main, Time spent computing the reference index: " << timeRefSketch.count() << " sec" << std::endl;

  //Map the sequences in query file
  t0 = skch::Time::now();

  skch::Map mapper = skch::Map(parameters, referSketch, spaced_seeds);

  std::chrono::duration<double> timeMapQuery = skch::Time::now() - t0;
  std::cout << "INFO, skch::main, Time spent mapping the query : " << timeMapQuery.count() << " sec" << std::endl;

  std::cout << "INFO, skch::main, mapping results saved in : " << parameters.outFileName << std::endl;

}
