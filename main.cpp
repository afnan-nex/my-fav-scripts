#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <iomanip>

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
    void execute(std::string folderPath, NamingPattern* pattern) {
        int count = 1;

        if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
            std::cout << "Error: Directory not found.\n";
            return;
        }

        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (fs::is_regular_file(entry)) {
                fs::path oldPath = entry.path();

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
    std::string path = "C:/Users/Admin/Desktop/Test_Dir_For_oop_Project"; // Ensure this folder exists!

    std::cout << "--- Batch Renamer Standardizer ---\n";

    // Choose your strategy (Polymorphism)
    NamingPattern* myStrategy = new SequencePattern("Vacation_Photos");

    renamer.execute(path, myStrategy);

    delete myStrategy; // Clean up memory
    return 0;
}
