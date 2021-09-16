#include <gtest/gtest.h>

#include <sstream>
#include <vector>
#include <string>
#include <iostream>

#define GTEST_COUT std::cerr << "[          ] [ INFO ]"

std::vector<std::string> tokenize(std::string input, char delimiter)
{
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    for (std::string each; std::getline(stream, each, delimiter); tokens.push_back(each));

    return tokens;
}

TEST(TokenTest, grouping)
{

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
        GTEST_COUT << "event" << std::endl;
        GTEST_COUT << event << std::endl;
        // split notes into duration and note
        auto durationSplit = tokenize(event, ':');
        auto notes = tokenize(durationSplit[0], '-');
        GTEST_COUT << "notes:" << std::endl;
        for (auto note : notes) {
            GTEST_COUT << note << std::endl;
        }
        auto duration = durationSplit[1];

        GTEST_COUT << "duration:" << duration << std::endl;
    }

    EXPECT_TRUE(1 == 1);
}
