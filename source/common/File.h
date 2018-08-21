#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include "RuntimeError.h"
#include "Buffer.h"

MAKE_PTR_TO(File) {
    private:
        Text path;
        struct stat stats;
        bool exists;

    public:
        File (const Text& _path) : path (_path), exists (stat (path, &stats) == 0) { }

        ~File () {}

        bool getExists () {
            return exists;
        }

        bool isDirectory () {
            return exists and S_ISDIR(stats.st_mode);
        }

        vector<PtrToFile> getFiles () {
            if (isDirectory ()) {
                vector<PtrToFile> files;
                DIR* directory = opendir (path);
                if (directory) {
                    struct dirent* ent;
                    while ((ent = readdir (directory))) {
                        Text name (ent->d_name);
                        if (name.find (".") != 0) {
                            files.push_back (new File (ent->d_name));
                        }
                    }
                    closedir (directory);
                }
                return files;
            }
            throw RuntimeError (Text ("File (") << path << ") is not a directory");
        }

        Text getPath () {
            return path;
        }

        Text getBasename () {
            vector<Text> components = path.split ("/");
            Text basename = components.back ();
            vector<Text> nameComponents = basename.split (".");
            if (nameComponents.size () > 1) {
                uint remove = nameComponents.back ().length () + 1;
                basename = basename.substring (0, basename.length () - remove);
            }
            return basename;
        }

        Text getExtension () {
            Text extension;
            vector<Text> components = path.split (getBasename () + ".");
            if (components.size () > 1) {
                extension = components.back ();
            }
            return extension;
        }

        PtrToBuffer read () {
            PtrToBuffer buffer;

            // open the file with the file pointer at the end of the file, and check if that worked
            ifstream file (path, ios::in | ios::binary | ios::ate);
            if (file.is_open()) {
                // get the size of the file and create the buffer to hold it,
                streampos size = file.tellg ();
                buffer = Buffer::make (size);

                // go back to the beginning of the file, and read the contents into the buffer, set
                // the buffer length to indicate success
                file.seekg (0, ios::beg);
                file.read ((char*) buffer->get (), size);
                buffer->setLength (size);

                // close the file before returning
                file.close();
            }
            return buffer;
        }

        Text readText () {
            return Text (read ());
        }

        // open, close
        // read, write
        // readText, writeText
        // read directory
        // delete
        // rename

};
