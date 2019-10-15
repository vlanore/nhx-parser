#include "nhx-parser.hpp"
#include <regex>

// lexer
void NHXParser::find_token() {
    std::map<TokenType, std::regex> token_regexes{{OpenParenthesis, std::regex("\\(")},
                                                  {CloseParenthesis, std::regex("\\)")},
                                                  {Colon, std::regex(":")},
                                                  {Semicolon, std::regex(";")},
                                                  {Comma, std::regex(",")},
                                                  {Equal, std::regex("=")},
                                                  {NHXOpen, std::regex("\\[&&NHX:")},
                                                  {CommentOpen, std::regex("\\[")},
                                                  {BracketClose, std::regex("\\]")},
                                                  {Identifier, std::regex("[a-zA-Z0-9._-]+")}};

    while (std::isspace(*it)) {
        it++;
    }
    int token_number{0};
    for (auto token_regex : token_regexes) {
        std::smatch m;
        auto r = std::regex_search(it, scit(input.end()), m, token_regex.second);
        if (r and m.prefix() == "") {
            if (token_regex.first == CommentOpen) {  // support of comments
                std::string comment_close{"]"};
                it =
                    std::search(it, scit(input.end()), comment_close.begin(), comment_close.end()) +
                    1;
                find_token();
                return;
            }
            next_token = Token{token_regex.first, m[0]};
            it += std::string(m[0]).size();
            // std::cout << "found token " << m[0] << std::endl;
            return;
        }
        token_number++;
    }
    next_token = Token{Invalid, it == scit(input.end())
                                    ? "end of input"
                                    : ("token starting with " + std::string(it, it + 1))};
}