#include <iostream>
#include <cstdarg>
#include <filesystem>
#include <unistd.h>
#include <boost/program_options.hpp>

// std::filesystem path to file
bool CheckFile(const std::filesystem::path& path, bool dir_write_required = true) {
    int read_access = access(path.c_str(), R_OK);
    if (read_access != 0) {
        return false;
    }
    if (dir_write_required) {
        auto directory_path = path.parent_path();
        if (directory_path.empty()) {
            directory_path = ".";
        }
        if (access(directory_path.c_str(), W_OK) != 0) {
            return false;
        }
    }
    return true;
}

bool CheckExistance(const std::filesystem::path& path) {
    return !access(path.c_str(), F_OK);
}

bool CheckSudo() {
    int me = getuid();
    int creds = geteuid();

    return me == 0 || me != creds;
}

void CallCommand(const std::string& command_name, const std::vector<std::string>& args,
                 bool as_sudo = false) {
    if (as_sudo) {
        std::cout << "Next file will be run as sudo. Are you sure?[y/n]\n";
        char c;
        std::cin >> c;
        if (c == 'y') {
            std::cout << "Continue operation\n";
        } else {
            std::cout << "Cancel operation\n";
            return;
        }
    }
    std::string arguments = " ";
    for (const auto& arg: args) {
        arguments += arg + " ";
    }
    std::string full = command_name + arguments;
    if (as_sudo) {
        full = "sudo " + full;
    }
    system(full.c_str());
}

int main(int argc, char** argv) {
    if (CheckSudo()) {
        std::cerr << "Probably, program called under sudo!\nExit\n";
        return 1;
    }
//    system("./hasher ../to_split");
    std::string filename;
    namespace po = boost::program_options;
    po::options_description desc;
    desc.add_options()
            ("help,h", "Help message")
            ("zip,z", po::bool_switch()->default_value(false), "Zip file in the same directory")
            ("hash,c", po::bool_switch()->default_value(false),
             "Get sha-256 hash of file and save in the same directory")
            ("split,s", po::bool_switch()->default_value(false),
             "Split file content basing on delimiter string,"
             " create directory and save multiple files  with content splited.")
            ("delim,d", po::value<std::string>()->default_value("\n"), "Delimiter for split")
            ("filename", po::value(&filename), "filename");
    po::positional_options_description desc_pos;
    desc_pos.add("filename", -1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(desc_pos).run(), vm);
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }
    if (!CheckExistance(vm["filename"].as<std::string>())) {
        std::cout << "Seems like file doesn't exist! Returining\n";
    }
    bool as_sudo = false;
    if (!CheckFile(vm["filename"].as<std::string>())) {
        std::cout << "Access problems! Next steps will require sudo!\n";
        as_sudo = true;
    }
    filename = vm["filename"].as<std::string>();
    if (vm["hash"].as<bool>()) {
        std::cout << "Getting hash...\n";
        CallCommand("./hasher", {filename}, as_sudo);
    }
    if (vm["zip"].as<bool>()) {
        std::cout << "Zipping your archive...\n";
        CallCommand("./dummy", {filename}, as_sudo);
    }
    if (vm["split"].as<bool>()) {
        std::vector<std::string> args{filename};
        if (vm.count("delim")) {
            std::string delim = vm["delim"].as<std::string>();
            args.push_back(delim);
        }
        std::cout << "Splitting!\n";
        CallCommand("./splitter", args, as_sudo);
    }
}
