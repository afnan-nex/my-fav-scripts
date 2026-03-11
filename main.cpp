#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <iomanip>
#include <map>

namespace fs = std::filesystem;

// 1. ABSTRACT BASE CLASS (Abstraction)
class NamingPattern {
public:
    // Pure Virtual Function: Every child MUST implement its own version
    virtual std::string generateName(const fs::path& originalPath, int index) = 0;
    virtual ~NamingPattern() {}
};

// 2. DERIVED CLASS: Prefix Pattern (Inheritance)
class PrefixPattern : public NamingPattern {
private:
    std::string prefix;
public:
    PrefixPattern(std::string p) : prefix(p) {}

    std::string generateName(const fs::path& originalPath, int index) override {
        // Result: Prefix_OriginalName.extension
        return prefix + "_" + originalPath.stem().string() + originalPath.extension().string();
    }
};

// 3. DERIVED CLASS: Sequence Pattern (Inheritance)
class SequencePattern : public NamingPattern {
private:
    std::string baseName;
public:
    SequencePattern(std::string bn) : baseName(bn) {}

    std::string generateName(const fs::path& originalPath, int index) override {
        // Result: BaseName_01.extension, BaseName_02.extension...
        return baseName + "_" + std::to_string(index) + originalPath.extension().string();
    }
};

// 4. THE ENGINE (Encapsulation)
class BatchRenamer {
public:
    void execute(std::string folderPath, NamingPattern* pattern, const std::string& targetExt = "") {
        int count = 1;

        if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
            std::cout << "Error: Directory not found.\n";
            return;
        }

        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (fs::is_regular_file(entry)) {
                fs::path oldPath = entry.path();

                // Filter by extension if specified
                if (!targetExt.empty() && oldPath.extension().string() != targetExt) {
                    continue;
                }

                // Polymorphism in action: calling the right pattern at runtime
                std::string newName = pattern->generateName(oldPath, count);
                fs::path newPath = oldPath.parent_path() / newName;

                try {
                    fs::rename(oldPath, newPath);
                    std::cout << "Renamed: " << oldPath.filename() << " -> " << newName << "\n";
                } catch (const fs::filesystem_error& e) {
                    std::cout << "Could not rename " << oldPath.filename() << ": " << e.what() << "\n";
                }
                count++;
            }
        }
    }
};

int main() {
    BatchRenamer renamer;
    std::string path;

    std::cout << "--- Batch Renamer Standardizer ---\n";
    std::cout << "Enter the folder path: ";
    std::getline(std::cin, path);

    // Convert forward slashes to backslashes for Windows paths
    for (char& c : path) {
        if (c == '/') {
            c = '\\';
        }
    }

    std::cout << "Using path: " << path << "\n\n";

    if (!fs::exists(path) || !fs::is_directory(path)) {
        std::cout << "Error: Directory not found.\n";
        return 1;
    }

    // Count files by extension
    std::map<std::string, std::vector<fs::path>> filesByExt;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (fs::is_regular_file(entry)) {
            std::string ext = entry.path().extension().string();
            filesByExt[ext].push_back(entry.path());
        }
    }

    // Display file counts by extension
    std::cout << "Files found in directory:\n";
    std::cout << "--------------------------\n";
    for (const auto& pair : filesByExt) {
        std::cout << pair.second.size() << " file" << (pair.second.size() > 1 ? "s" : "") 
                  << " of " << (pair.first.empty() ? "(no extension)" : pair.first) << " extension\n";
    }
    std::cout << "\n";

    // Ask user which extension to rename
    std::string targetExt;
    std::cout << "Enter the extension to rename (e.g., .jpg, .txt) or 'all' for all files: ";
    std::getline(std::cin, targetExt);

    // Convert forward slashes to backslashes for extension too
    for (char& c : targetExt) {
        if (c == '/') {
            c = '\\';
        }
    }

    // Ask for naming pattern
    std::cout << "\nChoose naming pattern:\n";
    std::cout << "1. Sequence (BaseName_01.ext, BaseName_02.ext...)\n";
    std::cout << "2. Prefix (Prefix_OriginalName.ext)\n";
    std::cout << "Enter choice (1 or 2): ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(); // Clear newline from buffer

    std::string nameInput;
    NamingPattern* myStrategy = nullptr;

    if (choice == 1) {
        std::cout << "Enter base name for sequence: ";
        std::getline(std::cin, nameInput);
        myStrategy = new SequencePattern(nameInput);
    } else if (choice == 2) {
        std::cout << "Enter prefix: ";
        std::getline(std::cin, nameInput);
        myStrategy = new PrefixPattern(nameInput);
    } else {
        std::cout << "Invalid choice. Exiting.\n";
        return 1;
    }

    std::cout << "\n--- Starting Rename ---\n\n";
    
    if (targetExt == "all") {
        renamer.execute(path, myStrategy);
    } else {
        renamer.execute(path, myStrategy, targetExt);
    }

    delete myStrategy; // Clean up memory
    return 0;
}
