#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <iomanip>
#include <map>
#include <sstream>

using namespace std;
namespace fs = std::filesystem;

// RESTART HELPER - compact and at the top
bool askRestart(const string& msg) {
    cout << "\n" << msg << "\nRestart? (y/n): ";
    char c; cin >> c; cin.ignore(1000, '\n');
    return (c == 'y' || c == 'Y');
}

// 1. ABSTRACT BASE CLASS (Abstraction)
class NamingPattern {
public:
    virtual string generateName(const fs::path& originalPath, int index) = 0;
    virtual ~NamingPattern() {}
};

// 2. DERIVED CLASS: Prefix Pattern (Inheritance)
class PrefixPattern : public NamingPattern {
    string prefix;
public:
    PrefixPattern(string p) : prefix(p) {}
    string generateName(const fs::path& originalPath, int index) override {
        return prefix + "_" + originalPath.stem().string() + originalPath.extension().string();
    }
};

// 3. DERIVED CLASS: Sequence Pattern (Inheritance)
class SequencePattern : public NamingPattern {
    string baseName;
public:
    SequencePattern(string bn) : baseName(bn) {}
    string generateName(const fs::path& originalPath, int index) override {
        ostringstream oss;
        oss << baseName << "_" << setw(3) << setfill('0') << index << originalPath.extension().string();
        return oss.str();
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
                if (!targetExt.empty() && oldPath.extension().string() != targetExt) continue;
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
restart: // RESTART POINT
    while (true) {
        BatchRenamer renamer;
        string path;

        cout << "\n=== Batch Renamer Standardizer ===\n";
        cout << "Enter folder path: ";
        getline(cin, path);
        for (char& c : path) if (c == '/') c = '\\';

        // ERROR CHECK: Path
        if (!fs::exists(path) || !fs::is_directory(path)) {
            if (askRestart("Error: Invalid directory.")) continue;
            else break;
        }

        // Count files
        map<string, vector<fs::path>> filesByExt;
        for (const auto& entry : fs::directory_iterator(path))
            if (fs::is_regular_file(entry))
                filesByExt[entry.path().extension().string()].push_back(entry.path());

        cout << "\nFiles found:\n";
        for (const auto& pair : filesByExt)
            cout << pair.second.size() << " file(s) of " << (pair.first.empty() ? "(no ext)" : pair.first) << "\n";

        cout << "\nFiles in directory:\n";
        for (const auto& entry : fs::directory_iterator(path))
            if (fs::is_regular_file(entry))
                cout << entry.path().filename().string() << "\n";

        // Get extension
        string targetExt;
        cout << "\nEnter extension to rename (e.g., .jpg) or 'all': ";
        getline(cin, targetExt);
        for (char& c : targetExt) if (c == '/') c = '\\';

        // ERROR CHECK: Extension exists
        if (targetExt != "all" && filesByExt.find(targetExt) == filesByExt.end()) {
            if (askRestart("Error: No files with that extension found.")) continue;
            else break;
        }

        // Get pattern choice
        cout << "\n1. Sequence (BaseName_001.ext)\n2. Prefix (Prefix_OriginalName.ext)\nChoice: ";
        int choice;
        cin >> choice;
        cin.ignore();

        // ERROR CHECK: Choice
        if (choice != 1 && choice != 2) {
            if (askRestart("Error: Invalid choice.")) continue;
            else break;
        }

        string nameInput;
        NamingPattern* myStrategy = nullptr;

        if (choice == 1) {
            cout << "Enter base name: ";
            getline(cin, nameInput);
            myStrategy = new SequencePattern(nameInput);
        } else {
            cout << "Enter prefix: ";
            getline(cin, nameInput);
            myStrategy = new PrefixPattern(nameInput);
        }

        // ERROR CHECK: Empty name
        if (nameInput.empty()) {
            if (askRestart("Error: Name cannot be empty.")) continue;
            else break;
        }

        cout << "\n--- Starting Rename ---\n";
        if (targetExt == "all") renamer.execute(path, myStrategy);
        else renamer.execute(path, myStrategy, targetExt);

        delete myStrategy;

        // Ask to run again
        if (!askRestart("Done! Run again?")) break;
    }

    cout << "\nGoodbye!\n";
    return 0;
}
