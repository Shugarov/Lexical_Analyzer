#include <stdexcept>
#include "lexical_analyzer.h"

lexical_analyzer::lexical_analyzer(const string& pr_text) {
    program_text = pr_text; // Присваиваем текст программы
    current_index = 0; // Инициализируем текущий индекс
}

vector<lexical_analyzer::lexeme> lexical_analyzer::get_data() {
    return data; // Возвращаем данные, полученные лексическим анализатором
}

void lexical_analyzer::run() {
    lexeme cur_lexeme; // Текущий лексема
    int cur_line = 1; // Текущая строка
    int cur_pos = 1; // Текущая позиция

    while (current_index < program_text.size()) { // Пока не достигнут конец текста программы
        while (isspace(program_text[current_index])) { // Пропускаем пробельные символы
            switch (program_text[current_index++]) {
            case ' ':
                ++cur_pos; // Увеличиваем позицию в строке
                break;
            case '\f':
            case '\n':
            case '\v':
                ++cur_line; cur_pos = 1; // Переходим на новую строку
                break;
            case '\t':
                cur_pos += 4; // Увеличиваем позицию на 4 (табуляция)
                break;
            }
        }

        cur_lexeme = next_lexeme(); // Получаем следующую лексему
        cur_lexeme.line = cur_line; // Устанавливаем номер строки для лексемы
        cur_lexeme.pos = cur_pos; // Устанавливаем позицию в строке для лексемы

        if (cur_lexeme.lexeme_type == lexemeType::ERROR) { // Если тип лексемы - ошибка
            string msg = "Analyzer error; line = " + to_string(cur_line) + ", pos = " + to_string(cur_pos); // Формируем сообщение об ошибке
            throw runtime_error(msg); // Выбрасываем исключение
        }

        cur_pos += cur_lexeme.value.size(); // Обновляем позицию в строке

        data.push_back(cur_lexeme); // Добавляем лексему в результат
    }

    cur_lexeme.lexeme_type = lexemeType::FINISH; // Устанавливаем тип лексемы "конец"
    cur_lexeme.line = cur_line; // Устанавливаем номер строки
    cur_lexeme.pos = cur_pos; // Устанавливаем позицию в строке
    data.push_back(cur_lexeme); // Добавляем лексему в результат
}

bool lexical_analyzer::ischar(char ch) {
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'); // Проверяем, является ли символ буквой
}

lexical_analyzer::lexeme lexical_analyzer::next_lexeme() {
    char cur_ch = program_text[current_index]; // Получаем текущий символ
    current_index++; // Увеличиваем индекс

    lexeme result; // Результат - лексема
    result.value = cur_ch; // Устанавливаем значение лексемы

    if (ischar(cur_ch)) { // Если символ - буква
        result.lexeme_type = lexemeType::VARIABLE; // Устанавливаем тип лексемы - переменная
        cur_ch = program_text[current_index]; // Получаем следующий символ
        while (current_index < program_text.size() && (ischar(cur_ch) || isdigit(cur_ch))) { // Пока символ - буква или цифра
            result.value += cur_ch; // Добавляем символ к значению лексемы
            current_index++; // Переходим к следующему символу
            cur_ch = program_text[current_index]; // Получаем следующий символ
        }
        // Проверяем ключевые слова
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
    else if (isdigit(cur_ch)) { // Если символ - цифра
        result.lexeme_type = lexemeType::NUMBER; // Устанавливаем тип лексемы - число
        cur_ch = program_text[current_index]; // Получаем следующий символ
        while (current_index < program_text.size() && isdigit(cur_ch)) { // Пока символ - цифра
            //result.value += cur_ch; // Добавляем символ к значению лексемы
            //current_index++; // Переходим к следующему символу
            //cur_ch = program_text[current_index]; // Получаем следующий символ
            result.value_int = result.value_int * 10 + (cur_ch - '0');
            current_index++;
            cur_ch = program_text[current_index];
        }
        // Проверяем наличие букв после числа (ошибка)
        if (current_index < program_text.size() && ischar(cur_ch)) {
            while (current_index < program_text.size() && ischar(cur_ch)) {
                result.value += cur_ch; // Добавляем символ к значению лексемы
                ++current_index; // Переходим к следующему символу
                cur_ch = program_text[current_index]; // Получаем следующий символ
            }
            result.lexeme_type = lexemeType::ERROR; // Устанавливаем тип лексемы - ошибка
        }
    }
    // Проверяем другие символы
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
        // Проверяем наличие '=' после '<'
        if (current_index < program_text.size() && program_text[current_index] == '=') {
            current_index++;
            result.lexeme_type = lexemeType::LESS_OR_EQUAL;
            result.value = "<=";
        }
    }
    else if (cur_ch == '=') {
        result.lexeme_type = lexemeType::ASSIGN;
        // Проверяем наличие '=' после '='
        if (current_index < program_text.size() && program_text[current_index] == '=') {
            current_index++;
            result.lexeme_type = lexemeType::EQUAL;
            result.value = "==";
        }
    }
    else if (cur_ch == '>') {
        result.lexeme_type = lexemeType::MORE;
        // Проверяем наличие '=' после '>'
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

    return result; // Возвращаем лексему
}
