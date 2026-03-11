#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <iomanip>
#include <map>

using namespace std;
namespace fs = std::filesystem;

// 1. ABSTRACT BASE CLASS (Abstraction)
class NamingPattern {
public:
    // Pure Virtual Function: Every child MUST implement its own version
    virtual string generateName(const fs::path& originalPath, int index) = 0;
    virtual ~NamingPattern() {}
};

// 2. DERIVED CLASS: Prefix Pattern (Inheritance)
class PrefixPattern : public NamingPattern {
private:
    string prefix;
public:
    PrefixPattern(string p) : prefix(p) {}

    string generateName(const fs::path& originalPath, int index) override {
        // Result: Prefix_OriginalName.extension
        return prefix + "_" + originalPath.stem().string() + originalPath.extension().string();
    }
};

// 3. DERIVED CLASS: Sequence Pattern (Inheritance)
class SequencePattern : public NamingPattern {
private:
    string baseName;
public:
    SequencePattern(string bn) : baseName(bn) {}

    string generateName(const fs::path& originalPath, int index) override {
        // Result: BaseName_01.extension, BaseName_02.extension...
        return baseName + "_" + to_string(index) + originalPath.extension().string();
    }
};

// 4. THE ENGINE (Encapsulation)
class BatchRenamer {
public:
    void execute(string folderPath, NamingPattern* pattern, const string& targetExt = "") {
        int count = 1;

        if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
            cout << "Error: Directory not found.\n";
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
                string newName = pattern->generateName(oldPath, count);
                fs::path newPath = oldPath.parent_path() / newName;

                try {
                    fs::rename(oldPath, newPath);
                    cout << "Renamed: " << oldPath.filename() << " -> " << newName << "\n";
                } catch (const fs::filesystem_error& e) {
                    cout << "Could not rename " << oldPath.filename() << ": " << e.what() << "\n";
                }
                count++;
            }
        }
    }
};

int main() {
    BatchRenamer renamer;
    string path;

    cout << "--- Batch Renamer Standardizer ---\n";
    cout << "Enter the folder path: ";
    getline(cin, path);

    // Convert forward slashes to backslashes for Windows paths
    for (char& c : path) {
        if (c == '/') {
            c = '\\';
        }
    }

    cout << "Using path: " << path << "\n\n";

    if (!fs::exists(path) || !fs::is_directory(path)) {
        cout << "Error: Directory not found.\n";
        return 1;
    }

    // Count files by extension
    map<string, vector<fs::path>> filesByExt;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (fs::is_regular_file(entry)) {
            string ext = entry.path().extension().string();
            filesByExt[ext].push_back(entry.path());
        }
    }

    // Display file counts by extension
    cout << "Files found in directory:\n";
    cout << "--------------------------\n";
    for (const auto& pair : filesByExt) {
        cout << pair.second.size() << " file" << (pair.second.size() > 1 ? "s" : "") 
                  << " of " << (pair.first.empty() ? "(no extension)" : pair.first) << " extension\n";
    }
    cout << "\n";

    // Ask user which extension to rename
    string targetExt;
    cout << "Enter the extension to rename (e.g., .jpg, .txt) or 'all' for all files: ";
    getline(cin, targetExt);

    // Convert forward slashes to backslashes for extension too
    for (char& c : targetExt) {
        if (c == '/') {
            c = '\\';
        }
    }

    // Ask for naming pattern
    cout << "\nChoose naming pattern:\n";
    cout << "1. Sequence (BaseName_01.ext, BaseName_02.ext...)\n";
    cout << "2. Prefix (Prefix_OriginalName.ext)\n";
    cout << "Enter choice (1 or 2): ";
    int choice;
    cin >> choice;
    cin.ignore(); // Clear newline from buffer

    string nameInput;
    NamingPattern* myStrategy = nullptr;

    if (choice == 1) {
        cout << "Enter base name for sequence: ";
        getline(cin, nameInput);
        myStrategy = new SequencePattern(nameInput);
    } else if (choice == 2) {
        cout << "Enter prefix: ";
        getline(cin, nameInput);
        myStrategy = new PrefixPattern(nameInput);
    } else {
        cout << "Invalid choice. Exiting.\n";
        return 1;
    }

    cout << "\n--- Starting Rename ---\n\n";
    
    if (targetExt == "all") {
        renamer.execute(path, myStrategy);
    } else {
        renamer.execute(path, myStrategy, targetExt);
    }

    delete myStrategy; // Clean up memory
    return 0;
}
