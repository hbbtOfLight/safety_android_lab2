//
// Created by katari on 4/16/23.
//
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <openssl/sha.h>


enum class HasherError {
    kNoError,
    kErrorOpeningInputFile,
    kErrorOpeningOutputFile
};
std::string GetSha(const std::string& inp) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, inp.c_str(), inp.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

HasherError GetHashFile(const std::filesystem::path& path_file) {
    std::filesystem::path path_hash = path_file;
    path_hash.replace_extension(".sha");
    std::ifstream fin(path_file);
    if (!fin.is_open()) {
        std::cerr << "Cannot open file input" << path_file << "\n";
        return HasherError::kErrorOpeningInputFile;
    }
    std::stringstream ss;
    ss << fin.rdbuf();
    std::string hash = GetSha(ss.str());
    std::ofstream fout (path_hash);
    if (!fout.is_open()) {
        std::cerr << "Error opening output file " << path_hash << "\n";
    }
    fout << hash;
//    std::cout << hash << "\n";
    return HasherError::kNoError;
}

int main(int argc, char** argv) {
    std::cout << "This is hasher!\n";
    if (argc != 2) {
        std::cerr << "Not enough args\n";
        return 1;
    }
    std::filesystem::path fpath(argv[1]);
    if (GetHashFile(fpath) != HasherError::kNoError) {
        std::cout << "Error getting hash!\n";
    } else {
        std::cout << "Success!\n";
    }
}
