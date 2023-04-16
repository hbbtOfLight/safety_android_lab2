//
// Created by katari on 4/16/23.
//

//creates from file directory where every file - splitted by delimiter part of content
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>


enum class SplitResult {
    kNoError,
    kSplitFailed,
    kErrorOnWrite
};

bool CheckEndsWith(const std::string& read, const std::string& delim) {
    if (read.size() < delim.size()) {
        return false;
    }
    bool entered = false;
    for (size_t i = read.size() + 1 - delim.size(), j = 0; i < read.size(); ++i, ++j) {
        entered = true;
        if (read[i] != delim[j]) {
            return false;
        }
    }
    if (!entered && delim.size() != 1) {
        return false;
    }
    return true;
}

std::vector<std::string>
Split(const std::filesystem::path& p, const std::string& delimiter = "\n") {
    std::ifstream fout(p);
    std::vector<std::string> outs;
    std::string current;
    while (fout.peek() != std::istream::traits_type::eof()) {
        std::string read;
        getline(fout, read, delimiter.front());
//        current += read + delimiter.front();
        size_t i = 1;
        bool ok_string = true;
        for (; i < delimiter.size() && fout.peek() != std::istream::traits_type::eof(); ++i) {
            char c = fout.get();
            if (c != delimiter[i]) {
                read += delimiter.substr(0, i);
                fout.putback(c);
                ok_string = false;
                break;
            }
        }
        current += read;
        if (ok_string) {
            outs.push_back(current);
            current.clear();
        }
//        if (CheckEndsWith(read, delimiter)) {
//            outs.push_back(current);
//            current.clear();
//        }
    }
    return outs;
}

SplitResult SplitFile(const std::filesystem::path& p, const std::string& delimiter = "\n") {
    auto files_contents = Split(p, delimiter);
    std::filesystem::path parent_dir = p.parent_path();
//    std::cout << parent_dir << "\n";
    if (parent_dir.empty()) {
        parent_dir = ".";
    }
    std::string filename = p.filename();
    std::string new_dir_name = filename + "_dir";
    std::filesystem::create_directory(parent_dir / new_dir_name);
    std::filesystem::path directory = parent_dir / (new_dir_name);
//    std::cout << directory << "\n";
//    std::cout << std::filesystem::current_path() << "\n";
    size_t id = 0;

    for (const auto& cont: files_contents) {
        std::ofstream fout(directory / (filename + std::to_string(id++)));
        if (!fout) {
            return SplitResult::kErrorOnWrite;
        }
        fout << cont;
    }
    return SplitResult::kNoError;
}


int main(int argc, char** argv) {
    std::cout << "Splitter startes work!\n";
    if (argc < 2 || argc > 3) {
        std::cerr << "Wrong arg number!\n";
        return 1;
    }
    SplitResult res;
    if (argc == 2) {
        res = SplitFile(argv[1]);
    } else {
        res = SplitFile(argv[1], std::string(argv[2]));
    }
    if (res == SplitResult::kNoError) {
        std::cout << "Success!\n";
    } else {
        std::cout << "Failed!\n";
    }
}