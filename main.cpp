#include <array>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

std::string exec(const char *cmd) {
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);

  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }

  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }

  return result;
}

std::string trim(const std::string &str) {
  size_t start = str.find_first_not_of(" \t\n\r");
  size_t end = str.find_last_not_of(" \t\n\r");
  return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

int main() {
  const std::string command = "gpspipe -w -n 10 | grep -m 1 '\"lat\":' | jq -r "
                              "'[.lat, .lon] | @csv' | tr -d '\"[]'";
  const std::string output_file = "output.data";

  std::ofstream file;
  bool first_write = true;

  std::cout << "Starting GPS data collection (every 2 seconds). Press Ctrl+C "
               "to stop.\n";

  while (true) {
    try {
      std::string output = exec(command.c_str());
      output = trim(output);

      if (!output.empty() && output.find(',') != std::string::npos) {

        file.open(output_file, std::ios::app);
        if (!file.is_open()) {
          std::cerr << "Error: Could not open " << output_file
                    << " for writing.\n";
          return 1;
        }

        if (!first_write) {
          file << ",";
        }

        file << output;
        file.close();
        first_write = false;

        std::cout << "Recorded: " << output << " to " << output_file
                  << std::endl;
      } else {
        std::cout << "No GPS fix or invalid data received.\n";
      }
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  return 0;
}
