#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>

#include <omp.h>
#include <dirent.h>

using namespace std;

class Config {
private:
  const static string DIRECTORY_PARAM;
  const static string FILE_PARAM;

public:
  vector<string> files;
  vector<string> keywords;

  Config(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
      string arg(argv[i]);

      // adding every file in the specified directory
      if (arg.substr(0, DIRECTORY_PARAM.length()) == DIRECTORY_PARAM) {
        string directory = arg.substr(
          DIRECTORY_PARAM.length(),
          arg.length()
        );

        addFiles(directory);
      }
      // adding a specified file
      else if (arg.substr(0, FILE_PARAM.length()) == FILE_PARAM) {
        string file = arg.substr(
          FILE_PARAM.length(),
          arg.length()
        );

        files.push_back(file);
      }
      // adding a keywords to search for
      else {
        keywords.push_back(arg);
      }
    }
  }

private:
  void addFiles(string directory) {
    DIR *dir;
    struct dirent *entity;

    if ((dir = opendir(directory.c_str())) != NULL) {
      while ((entity = readdir(dir)) != NULL) {
        string file = string(entity->d_name);

        if (file != "." && file != "..") {
          file = directory + "/" + file;
          files.push_back(file);
        }
      }

      closedir(dir);
    }
  }
};

const string Config::DIRECTORY_PARAM = "--dir=";
const string Config::FILE_PARAM = "--file=";

class ReverseIndex {
private:
  const static int CACHE_SPREAD;

public:
  ReverseIndex(Config config) {
    cout << "Files: " << endl;
    for (string file : config.files) {
      cout << " - " << file << endl;
    }

    cout << endl << "Keywords: " << endl;
    for (string keyword : config.keywords) {
      cout << " - " << keyword << endl;
    }

    map<string, vector<string>> index = recreateIndex(config.files);
    find(index, config.keywords);
  }

private:
  map<string, vector<string>> recreateIndex(vector<string> files) {
    int threads = omp_get_max_threads();

    vector<map<string, vector<string>>> results((unsigned long) threads * CACHE_SPREAD);

    // processing file in parallel
    #pragma omp parallel for shared(results)
    for (int i = 0; i < files.size(); i++) {
      int thread = omp_get_thread_num();

      string file = files[i];
      map<string, vector<string>>* keywords = &results[thread * CACHE_SPREAD];

      // file loading and processing
      ifstream handle(file);
      string keyword;

      while (handle >> keyword) {
        // creating a list of files for given keyword if it does not exist
        if (keywords->count(keyword) == 0) {
          keywords->insert({ keyword, vector<string>() });
        }

        vector<string>* fileNames = &keywords->at(keyword);

        // adding a file to the list, if the file does not already exist
        if(std::find(fileNames->begin(), fileNames->end(), file) == fileNames->end()) {
          keywords->at(keyword).push_back(file);
        }
      }

      handle.close();
    }

    map<string, vector<string>> index;

    for (int i = 0; i < threads; i++) {
      map<string, vector<string>> result = results[i * CACHE_SPREAD];

      for (pair<string, vector<string>> pair : result) {
        string keyword = pair.first;
        vector<string> sourceFileNames = pair.second;

        if (index.count(keyword) == 0) {
          index.insert({ keyword, vector<string>() });
        }

        vector<string>* fileNames = &index.at(keyword);

        fileNames->insert(
                fileNames->end(),
                sourceFileNames.begin(),
                sourceFileNames.end()
        );
      }
    }

    return index;
  }

  void find(map<string, vector<string>> index, vector<string> keywords) {
    int threads = omp_get_max_threads();

    vector<vector<pair<string, vector<string>>>> results((unsigned int) threads * CACHE_SPREAD);

    #pragma omp parallel for shared(results, index, keywords)
    for(int i = 0; i < index.size(); i++) {
      map<string, vector<string>>::iterator it = index.begin();
      advance(it, i);

      string keyword = it->first;

      if(std::find(keywords.begin(), keywords.end(), keyword) != keywords.end()) {
        int thread = omp_get_thread_num();

        results[thread * CACHE_SPREAD].push_back({ it->first, it->second });
      }
    }

    cout << endl;
    for (int i = 0; i < threads; i++) {
      vector<pair<string, vector<string>>> v = results[i * CACHE_SPREAD];

      for (pair<string, vector<string>> pair : v) {
        cout << pair.first << ":" << endl;

        for (string file : pair.second) {
          cout << " - " << file << endl;
        }

        cout << endl;
      }
    }
  }
};

const int ReverseIndex::CACHE_SPREAD = 64;

int main(int argc, char* argv[]) {
  Config config(argc, argv);
  ReverseIndex reverseIndex(config);
}
