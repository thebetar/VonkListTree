#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <cmath>

using namespace std;
namespace fs = std::filesystem;

string get_indent(int depth, bool is_last, const vector<bool> &is_last_at_depth)
{
    string indent;

    for (int i = 0; i < depth; ++i)
    {
        indent += (is_last_at_depth[i] ? "    " : "│   ");
    }

    indent += (is_last ? "└── " : "├── ");
    return indent;
}

string get_size(fs::directory_entry entry)
{
    try
    {

        string size;

        if (entry.is_regular_file())
        {
            uintmax_t file_size = entry.file_size();

            if (file_size >= 10e9)
            {
                const unsigned int file_size_gb = file_size / 1e9;
                size = to_string(file_size_gb) + "GB";
            }
            else if (file_size >= 10e6)
            {
                const unsigned int file_size_mb = file_size / 1e6;
                size = to_string(file_size_mb) + "MB";
            }
            else if (file_size >= 10e3)
            {
                const unsigned int file_size_kb = file_size / 1e3;
                size = to_string(file_size_kb) + "KB";
            }
            else
            {
                size = to_string(file_size) + "B";
            }
        }
        else if (entry.is_directory())
        {
            size = "(DIR)";
        }

        // Make sure the output is 10 characters long
        size = size + string(10 - size.size(), ' ');

        return size;
    }
    catch (const exception &e)
    {
        cout << "An error occurred: " << e.what() << endl;
        return "ERROR" + string(5, ' ');
    }
}

void print_file_info(fs::directory_entry entry, int depth, bool is_last, const vector<bool> &is_last_at_depth)
{
    // Get the file size
    string output = get_size(entry);
    // Get the indentation
    output += get_indent(depth, is_last, is_last_at_depth);
    // Get the file name
    output += entry.path().filename().string();
    // Print the file name
    cout << output << endl;
}

int get_files(string path, int maxdepth, int depth = 0, vector<bool> is_last_at_depth = vector<bool>())
{
    // Store all the entries in the directory
    vector<fs::directory_entry> entries;

    // Iterate through the directory
    for (const auto &entry : fs::directory_iterator(path))
    {
        // Store the entry
        entries.push_back(entry);
    }

    // If the depth is greater than the size of the vector
    if (depth >= is_last_at_depth.size())
    {
        // Add a new element to the vector
        is_last_at_depth.push_back(false);
    }

    // Iterate through the entries
    for (size_t i = 0; i < entries.size(); ++i)
    {
        // Check if the current entry is the last one
        bool is_last = (i == entries.size() - 1);
        is_last_at_depth[depth] = is_last;

        // If file
        if (fs::is_regular_file(entries[i].path()))
        {
            print_file_info(entries[i], depth, is_last, is_last_at_depth);
        }
        else if (fs::is_directory(entries[i].path()))
        {
            print_file_info(entries[i], depth, is_last, is_last_at_depth);

            if (maxdepth == -1 || depth < maxdepth)
            {
                get_files(entries[i].path().string(), maxdepth, depth + 1, is_last_at_depth);
            }
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    // Get the current path from where the program is called
    string path = fs::current_path().string();
    cout << "Current path: " << path << endl;

    int maxdepth = -1;

    // Check for maxdepth argument
    if (argc > 1)
    {
        for (int i = 0; i < argc; i++)
        {
            if (string(argv[i]) == "--maxdepth")
            {
                maxdepth = stoi(argv[i + 1]);
            }
            else if (string(argv[i]) == "--path")
            {
                path = argv[i + 1];
            }
            else if (string(argv[i]) == "--help")
            {
                cout << "Usage: list-tree [--maxdepth <depth>] [--path <path>]" << endl;
                return 0;
            }
        }
    }

    printf("Maxdepth: %d\n", maxdepth);

    // Get the files in the current path
    get_files(path, maxdepth);

    return 0;
}