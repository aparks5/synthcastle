#include <sstream>
#include <vector>
#include <string>
#include <iostream>

std::vector<std::string> tokenize(std::string input, char delimiter)
{
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    for (std::string each; std::getline(stream, each, delimiter); tokens.push_back(each));

    return tokens;
}


int main() {
    // inputs
    // need multiple tokenizers
    // dashes connect notes so they form a chord
    // : indicates duration
    std::string str("63-68-69:0.5,70:0.25");
    char noteGroup = '-';
    char eventGroup = ',';
    char durationSpecifier = ':';

    // get events first
    auto events = tokenize(str, ',');
    // now use `events`
    for (auto event : events) {
        std::cout << "event" << std::endl;
        std::cout << event << std::endl;
        // split notes into duration and note
        auto durationSplit = tokenize(event, ':');
        auto notes = tokenize(durationSplit[0], '-');
        std::cout << "notes:" << std::endl;
        for (auto note : notes) {
            std::cout << note << std::endl;
        }
        auto duration = durationSplit[1];
        std::cout << "duration:" << duration << std::endl;
        std::cout << "*** end event ***" << std::endl;
    }

}
