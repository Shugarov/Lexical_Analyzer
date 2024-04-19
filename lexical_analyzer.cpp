
#include <stdexcept>
#include "lexical_analyzer.h"

lexical_analyzer::lexical_analyzer(const string& pr_text) {
    program_text = pr_text;
    current_index = 0;
}

vector<lexical_analyzer::lexeme> lexical_analyzer::get_data() {
    return data;
}

void lexical_analyzer::run() {
    lexeme cur_lexeme;
    int cur_line = 1;
    int cur_pos = 1;

    while (current_index < program_text.size()) {
        while (isspace(program_text[current_index])) {
            switch (program_text[current_index++]) {
            case ' ':
                ++cur_pos;
                break;
            case '\f':
            case '\n':
            case '\v':
                ++cur_line; cur_pos = 1;
                break;
            case '\t':
                cur_pos += 4;
                break;
            }
        }

        cur_lexeme = next_lexeme();
        cur_lexeme.line = cur_line;
        cur_lexeme.pos = cur_pos;

        if (cur_lexeme.lexeme_type == lexemeType::ERROR) {
            string msg = "Analyzer error; line = " + to_string(cur_line) + ", pos = " + to_string(cur_pos);
            throw runtime_error(msg);
        }

        cur_pos += cur_lexeme.value.size();

        data.push_back(cur_lexeme);
    }

    cur_lexeme.lexeme_type = lexemeType::FINISH;
    cur_lexeme.line = cur_line;
    cur_lexeme.pos = cur_pos;
    data.push_back(cur_lexeme);
}

bool lexical_analyzer::ischar(char ch) {
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
}

lexical_analyzer::lexeme lexical_analyzer::next_lexeme() {
    char cur_ch = program_text[current_index];
    current_index++;

    lexeme result;
    result.value = cur_ch;

    if (ischar(cur_ch)) {
        result.lexeme_type = lexemeType::VARIABLE;
        cur_ch = program_text[current_index];
        while (current_index < program_text.size() && ischar(cur_ch) || isdigit(cur_ch)) {
            result.value += cur_ch;
            current_index++;
            cur_ch = program_text[current_index];
        }
        if (result.value == "int") {
            result.lexeme_type = lexemeType::INT;
        }
        else if (result.value == "array") {
            result.lexeme_type = lexemeType::ARRAY;
        }
        else if (result.value == "if") {
            result.lexeme_type = lexemeType::IF;
        }
        else if (result.value == "else") {
            result.lexeme_type = lexemeType::ELSE;
        }
        else if (result.value == "while") {
            result.lexeme_type = lexemeType::WHILE;
        }
        else if (result.value == "read") {
            result.lexeme_type = lexemeType::READ;
        }
        else if (result.value == "write") {
            result.lexeme_type = lexemeType::WRITE;
        }
    }
    else if (isdigit(cur_ch)) {
        result.lexeme_type = lexemeType::NUMBER;
        cur_ch = program_text[current_index];
        while (current_index < program_text.size() && isdigit(cur_ch)) {
            result.value += cur_ch;
            current_index++;
            cur_ch = program_text[current_index];
        }
        if (current_index < program_text.size() && ischar(cur_ch)) {
            while (current_index < program_text.size() && ischar(cur_ch)) {
                result.value += cur_ch;
                ++current_index;
                cur_ch = program_text[current_index];
            }
            result.lexeme_type = lexemeType::ERROR;
        }
    }
    else if (cur_ch == '{') {
        result.lexeme_type = lexemeType::LEFT_BRACE;
    }
    else if (cur_ch == '}') {
        result.lexeme_type = lexemeType::RIGHT_BRACE;
    }
    else if (cur_ch == '[') {
        result.lexeme_type = lexemeType::LEFT_SQUARE_BRACKET;
    }
    else if (cur_ch == ']') {
        result.lexeme_type = lexemeType::RIGHT_SQUARE_BRACKET;
    }
    else if (cur_ch == '(') {
        result.lexeme_type = lexemeType::LEFT_ROUND_BRACKET;
    }
    else if (cur_ch == ')') {
        result.lexeme_type = lexemeType::RIGHT_ROUND_BRACKET;
    }
    else if (cur_ch == '+') {
        result.lexeme_type = lexemeType::PLUS;
    }
    else if (cur_ch == '-') {
        result.lexeme_type = lexemeType::MINUS;
    }
    else if (cur_ch == '*') {
        result.lexeme_type = lexemeType::MULTIPLY;
    }
    else if (cur_ch == '/') {
        result.lexeme_type = lexemeType::DIVIDE;
    }
    else if (cur_ch == ';') {
        result.lexeme_type = lexemeType::SEMICOLON;
    }
    else if (cur_ch == ',') {
        result.lexeme_type = lexemeType::COMMA;
    }
    else if (cur_ch == '<') {
        result.lexeme_type = lexemeType::LESS;
        if (current_index < program_text.size() && program_text[current_index] == '=') {
            current_index++;
            result.lexeme_type = lexemeType::LESS_OR_EQUAL;
            result.value = "<=";
        }
    }
    else if (cur_ch == '=') {
        result.lexeme_type = lexemeType::ASSIGN;
        if (current_index < program_text.size() && program_text[current_index] == '=') {
            current_index++;
            result.lexeme_type = lexemeType::EQUAL;
            result.value = "==";
        }
    }
    else if (cur_ch == '>') {
        result.lexeme_type = lexemeType::MORE;
        if (current_index < program_text.size() && program_text[current_index] == '=') {
            current_index++;
            result.lexeme_type = lexemeType::MORE_OR_EQUAL;
            result.value = ">=";
        }
    }
    else if (cur_ch == '!' && current_index + 1 < program_text.size() && program_text[current_index + 1] == '=') {
        current_index++;
        result.lexeme_type = lexemeType::NOT_EQUAL;
        result.value = "!=";
    }

    return result;
}

