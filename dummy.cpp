//
// Created by katari on 4/16/23.
//
#include <unistd.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <zip.h>

enum class ZipFuncErrors {
    kNoerr,
    kErrOpeningFile,
    kErrOpeningArchive,
    kErrOpeningStream,
    kErrAddingFile
};


bool CheckSudo() {
    int me = getuid();
    int creds = geteuid();

    return me == 0 || me != creds;
}

ZipFuncErrors ZipifyArchive(const std::filesystem::path& path_to_file) {
    std::filesystem::path path_to_zip = path_to_file;
    path_to_zip.replace_extension(".zip");
    int zip_err;
    std::ifstream fin(path_to_file);
    if (!fin.is_open()) {
        std::cerr << "Cannot open file at " << path_to_file.c_str() << "\n";
        return ZipFuncErrors::kErrOpeningFile;
    }
    std::stringstream ss;
    ss << fin.rdbuf();
    std::string data_string = ss.str();
    auto archive = zip_open(path_to_zip.c_str(), ZIP_CREATE, &zip_err);
    if (!archive) {
        std::cerr << "Error opening archive " << path_to_zip.c_str() << "\n";
        return ZipFuncErrors::kErrOpeningArchive;
    }
    zip_source* s = zip_source_buffer(archive, data_string.c_str(), data_string.size(), 0);
    if (!s) {
        std::cerr << "Error creating source\n";
        return ZipFuncErrors::kErrOpeningStream;
    }
    if (zip_file_add(archive, path_to_file.filename().c_str(), s, ZIP_FL_ENC_UTF_8)) {
        std::cerr << "Error adding file\n";
        return ZipFuncErrors::kErrAddingFile;
    }
    zip_close(archive);
    return ZipFuncErrors::kNoerr;
}

std::filesystem::path GetAbsolute(const char* str_path) {
    std::filesystem::path fs_path(str_path);
    if (fs_path.is_relative()) {
        return absolute(fs_path);
    }
    return fs_path;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Wrong call! Not enough args!\n";
        return 1;
    }
    std::cout << "Starting creating zip archive from " << argv[1] << "...\n";
    auto err = ZipifyArchive(std::filesystem::path(argv[1]));
    if (err == ZipFuncErrors::kNoerr) {
        std::cout << "Success!\n";
    } else {
        std::cout << "Error happened!\n";
    }
    return 0;
}

