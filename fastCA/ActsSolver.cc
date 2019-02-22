#include "ActsSolver.h"
#include <unistd.h>

void ActsSolver::solve(const SpecificationFile &specificationFile,
                       const ConstraintFile &constraintFile,
                       const std::string &res_file_name) {
  char filename[L_tmpnam];
  if (!tmpnam(filename)) {
    std::cerr << "tmp file name error" << std::endl;
  }
  std::string acts_inputfile_name = filename;
  acts_inputfile_name += std::to_string(getpid());
  acts_inputfile_name += ".txt";
  generate_input_file(specificationFile, constraintFile, acts_inputfile_name);
  const unsigned strenth = specificationFile.getStrenth();
  std::string cmd =
      "java -Ddoi=" + std::to_string(strenth) + " -jar acts_cmd_2.92.jar ";
  cmd += std::string("cmd ") + acts_inputfile_name + " " + res_file_name;
  std::string rm_cmd = std::string("rm ") + acts_inputfile_name;
  if (system(cmd.c_str()) != 0 || system(rm_cmd.c_str()) != 0) {
    std::cout << "system() wrong" << std::endl;
    exit(0);
  };
}

void ActsSolver::generate_input_file(const SpecificationFile &specificationFile,
                                     const ConstraintFile &constraintFile,
                                     const std::string &acts_inputfile_name) {
  std::ofstream acts_infile(acts_inputfile_name);
  if (!acts_infile.is_open()) {
    std::cerr << "open failed" << std::endl;
    exit(0);
  }

  acts_infile << "[System]" << std::endl;
  acts_infile << "Name: " << acts_inputfile_name << std::endl << std::endl;
  acts_infile << "[Parameter]" << std::endl;

  const Options &options = specificationFile.getOptions();
  for (unsigned option = 0; option < options.size(); ++option) {
    acts_infile << 'p' << option << "(int): ";
    acts_infile << 0;
    for (unsigned var = 1; var < options.symbolCount(option); ++var) {
      acts_infile << ',' << var;
    }
    acts_infile << std::endl;
  }

  acts_infile << std::endl << "[Constraint]" << std::endl;
  const Valid::Formula &formula = constraintFile.getFormula();
  for (auto &c : formula) {
    const unsigned option = options.option(c[0].variable());
    acts_infile << 'p' << option;
    c[0].is_negative() ? acts_infile << "!=" : acts_infile << "=";
    acts_infile << c[0].variable() - options.firstSymbol(option);
    for (unsigned i = 1; i < c.size(); ++i) {
      acts_infile << " || ";
      const unsigned option = options.option(c[i].variable());
      acts_infile << 'p' << option;
      c[i].is_negative() ? acts_infile << "!=" : acts_infile << "=";
      acts_infile << c[i].variable() - options.firstSymbol(option);
    }
    acts_infile << ' ' << std::endl;
  }
  acts_infile.close();
}
