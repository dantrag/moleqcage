#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>

#include "caging3d/cagingjob.h"

void ShowHelp() {
  std::cout << "Usage: ./NCaging3D -g grid_file -o object_file -o obstacle_file [options]" << std::endl << std::endl;
  std::cout << "options:" << std::endl;
  std::cout << " -h                  display this message" << std::endl << std::endl;
  std::cout << " -g grid_file        file containing SO3 grid samples" << std::endl;
  std::cout << " -o balls_file       file containing balls-based representation of an object or an obstacle" << std::endl;
  std::cout << " -e epsilon          epsilon value for CagingParameters (default is 1.0)" << std::endl;
  std::cout << " -i infinity         infinity value for CagingParameters (default is 1e5)" << std::endl;
  std::cout << " -f [output_file]    request writing results to a file (default is result.txt)" << std::endl << std::endl;
  std::cout << "For the exact file formats, see examples" << std::endl;
}

std::pair<char*, char*> get_argument(char** arg_begin, char** arg_end, const std::string& key) {
  auto result = std::make_pair((char*)0, (char*)0);
  auto arg = std::find(arg_begin, arg_end, key);
  if (arg != arg_end) {
    result.first = *arg;
    if (++arg != arg_end) {
      result.second = *arg;
      return result;
    } else {
      result.first = 0;
    }
  }
  return result;
}

bool find_argument(char** arg_begin, char** arg_end, const std::string& key) {
  return std::find(arg_begin, arg_end, key) != arg_end;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i) {
    std::stringstream stm;
    stm << argv[i];
    std::string str;
    stm >> str;
    args.push_back(str);
  }
  if (std::find(args.begin(), args.end(), "-h") != args.end()) {
    ShowHelp();
  } else {
    caging3d::CagingParameters params;
    auto eps_arg = std::find(args.begin(), args.end(), "-e");
    if (eps_arg != args.end() && ++eps_arg != args.end()) {
      std::stringstream stm;
      stm << *eps_arg;
      double eps;
      stm >> eps;
      params.SetEpsilon(eps);
    }
    auto inf_arg = std::find(args.begin(), args.end(), "-i");
    if (inf_arg != args.end() && ++inf_arg != args.end()) {
      std::stringstream stm;
      stm << *inf_arg;
      double inf;
      stm >> inf;
      params.SetInfinity(inf);
    }
    auto file_arg = std::find(args.begin(), args.end(), "-f");
    std::string output_file = (file_arg == args.end()) ? "" : "result.txt";
    if (file_arg != args.end() && ++file_arg != args.end()) {
      output_file = *file_arg;
    }
    auto grid_arg = std::find(args.begin(), args.end(), "-g");
    std::string grid = "";
    if (grid_arg != args.end() && ++grid_arg != args.end()) {
      grid = *grid_arg;
    } else {
      std::cout << "Mandatory parameter -g is missing!" << std::endl;
      return 0;
    }
    auto obj_arg = std::find(args.begin(), args.end(), "-o");
    if (obj_arg != args.end() && ++obj_arg != args.end()) {
      auto obs_arg = std::find(obj_arg, args.end(), "-o");
      if (obs_arg != args.end() && ++obs_arg != args.end()) {
        auto object = *obj_arg;
        auto obstacle = *obs_arg;
        caging3d::CagingJob job(object, obstacle, grid, params);
        auto result = job.Run();
        if (!output_file.empty()) {
          job.PrintToFile(output_file);
        }
        auto components = result.components();
        std::cout << components.size() << " components:" << std::endl;
        for (size_t i = 0; i < components.size(); ++i) {
          std::cout << " " << i << ": " << components[i] << std::endl;
        }
        std::cout << "Time: " << result.TimeInMs() << " ms" << std::endl;
      } else {
        std::cout << "Second mandatory parameter -o is missing!" << std::endl;
      }
    } else {
      std::cout << "Mandatory parameter -o is missing!" << std::endl;
      return 0;
    }
  }

  return 0;
}

