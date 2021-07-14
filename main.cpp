/*********************************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Youtao Lu@Kim Lab, 2016-2020
*********************************************************************************/
#define VERSION 0.2
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_map>
#include <array>
#include <cmath>

// http://www.martinbroadhurst.com/how-to-split-a-string-in-c.html
template <class Container>
void split(const std::string& str, Container& cont, char delim = ' ') {
    std::size_t current, previous = 0;
    current = str.find(delim);
    while (current != std::string::npos) {
        cont.push_back(str.substr(previous, current - previous));
        previous = current + 1;
        current = str.find(delim, previous);
    }
    cont.push_back(str.substr(previous, current - previous));
}
 
// insert size is column 9 (TLEN); 
// note: only count the positive one as zero means singleton and negative means 
// a mate sitting in the position closer to the end of the chromosome; 

int main() {
    std::unordered_map<std::string, std::array<int, 2> > stats;
    for (std::string line; std::getline(std::cin, line);) {
        std::vector<std::string> words;
        split(line, words, '\t');
        std::string readID = words.at(0);
        int l = std::stoi(words.at(8));
        if (l <= 0) {
            continue;
        }
        std::string ascore = words.at(13);
        std::vector<std::string> as;
        split(ascore, as, ':');
        int s = std::stoi(as.at(2));
        std::unordered_map<std::string, std::array<int, 2> >::const_iterator find = stats.find(readID);
        if (find == stats.end()) {
            std::array<int, 2> stat = { l, s };
            stats[readID] = stat;
        } else {
            std::array<int, 2> stat = stats[readID];
            if (stat.at(1) < s) { 
                // current read has better score than the old one; stat needs to be updated;
                stat = { l, s }; 
                stats[readID] = stat;
            }
        }
    }
    
    std::vector<int> tlen;
    int min = 65535, max = -1;
    double mean = 0.0;
    double sd = 0.0;
    double median = 0.0;
    double q1 = 0.0, q3 = 0.0;
    unsigned long n = stats.size();

    for (auto& stat : stats) {
        tlen.push_back(stat.second.at(0));
    }

    for (auto& l : tlen) {
        mean += l;
        if (l < min) {
            min = l;
        }
        if (l > max) {
            max = l;
        }
    }
    mean /= n;

    for (auto& l : tlen) {
        sd += (l - mean) * (l - mean);
    }
    sd = std::sqrt(sd / (n - 1));

    std::sort(tlen.begin(), tlen.end());
    
    if (n % 2 == 0) {
        median = 0.5 * (tlen.at(n/2 - 1) + tlen.at(n/2));
    } else {
        median = tlen.at((n - 1)/2);
    }
    q1 = tlen.at(int(n * 0.25));
    q3 = tlen.at(int(n * 0.75));

    std::cout << "Min." << "\t" << "1st Qu." << "\t" << "Median" << "\t" << "Mean" << "\t" << "3rd Qu." << "\t" << "Max." << "\t" << "SD" << std::endl;
    std::cout << min << "\t" << q1 << "\t" << round(median) << "\t" << round(mean) << "\t" << q3 << "\t" << max << "\t" << sd << std::endl;

    return 0;
}
