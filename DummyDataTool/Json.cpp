#include "Json.h"

#include <cctype>
#include <stdexcept>

double JsonValue::AsDouble() const {
    return numberValue;
}

int JsonValue::AsInt() const {
    return static_cast<int>(numberValue);
}

const std::string& JsonValue::AsString() const {
    return stringValue;
}

const std::vector<JsonValue>& JsonValue::AsArray() const {
    return arrayValue;
}

const JsonValue& JsonValue::Get(const std::string& key) const {
    static const JsonValue nullValue;
    auto it = objectValue.find(key);
    if (it == objectValue.end()) {
        return nullValue;
    }
    return it->second;
}

namespace {

class Cursor {
public:
    explicit Cursor(const std::string& text) : text_(text), pos_(0) {}

    JsonValue ParseValue() {
        SkipWhitespace();
        if (pos_ >= text_.size()) {
            throw std::runtime_error("JSON 파싱 오류: 예상치 못한 문서 끝");
        }
        switch (text_[pos_]) {
            case '{': return ParseObject();
            case '[': return ParseArray();
            case '"': return ParseString();
            case 't':
            case 'f': return ParseBool();
            case 'n': return ParseNull();
            default: return ParseNumber();
        }
    }

private:
    const std::string& text_;
    size_t pos_;

    void SkipWhitespace() {
        while (pos_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[pos_]))) {
            ++pos_;
        }
    }

    char Peek() const { return text_[pos_]; }

    void Expect(char c) {
        if (pos_ >= text_.size() || text_[pos_] != c) {
            throw std::runtime_error(std::string("JSON 파싱 오류: '") + c + "' 문자가 필요합니다");
        }
        ++pos_;
    }

    JsonValue ParseObject() {
        JsonValue value;
        value.type = JsonType::Object;
        Expect('{');
        SkipWhitespace();
        if (pos_ < text_.size() && Peek() == '}') {
            ++pos_;
            return value;
        }
        while (true) {
            SkipWhitespace();
            JsonValue key = ParseString();
            SkipWhitespace();
            Expect(':');
            JsonValue val = ParseValue();
            value.objectValue[key.stringValue] = val;
            SkipWhitespace();
            if (pos_ < text_.size() && Peek() == ',') {
                ++pos_;
                continue;
            }
            break;
        }
        SkipWhitespace();
        Expect('}');
        return value;
    }

    JsonValue ParseArray() {
        JsonValue value;
        value.type = JsonType::Array;
        Expect('[');
        SkipWhitespace();
        if (pos_ < text_.size() && Peek() == ']') {
            ++pos_;
            return value;
        }
        while (true) {
            value.arrayValue.push_back(ParseValue());
            SkipWhitespace();
            if (pos_ < text_.size() && Peek() == ',') {
                ++pos_;
                continue;
            }
            break;
        }
        SkipWhitespace();
        Expect(']');
        return value;
    }

    JsonValue ParseString() {
        JsonValue value;
        value.type = JsonType::String;
        Expect('"');
        std::string result;
        while (pos_ < text_.size() && text_[pos_] != '"') {
            char c = text_[pos_];
            if (c == '\\' && pos_ + 1 < text_.size()) {
                ++pos_;
                char esc = text_[pos_];
                switch (esc) {
                    case 'n': result += '\n'; break;
                    case 't': result += '\t'; break;
                    case 'r': result += '\r'; break;
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    default: result += esc; break;
                }
            } else {
                result += c;
            }
            ++pos_;
        }
        Expect('"');
        value.stringValue = result;
        return value;
    }

    JsonValue ParseNumber() {
        size_t start = pos_;
        if (pos_ < text_.size() && (text_[pos_] == '-' || text_[pos_] == '+')) {
            ++pos_;
        }
        while (pos_ < text_.size() &&
               (std::isdigit(static_cast<unsigned char>(text_[pos_])) || text_[pos_] == '.' ||
                text_[pos_] == 'e' || text_[pos_] == 'E' || text_[pos_] == '-' || text_[pos_] == '+')) {
            ++pos_;
        }
        JsonValue value;
        value.type = JsonType::Number;
        value.numberValue = std::stod(text_.substr(start, pos_ - start));
        return value;
    }

    JsonValue ParseBool() {
        JsonValue value;
        value.type = JsonType::Bool;
        if (text_.compare(pos_, 4, "true") == 0) {
            value.boolValue = true;
            pos_ += 4;
        } else if (text_.compare(pos_, 5, "false") == 0) {
            value.boolValue = false;
            pos_ += 5;
        } else {
            throw std::runtime_error("JSON 파싱 오류: 잘못된 bool 리터럴");
        }
        return value;
    }

    JsonValue ParseNull() {
        if (text_.compare(pos_, 4, "null") != 0) {
            throw std::runtime_error("JSON 파싱 오류: 잘못된 null 리터럴");
        }
        pos_ += 4;
        return JsonValue{};
    }
};

}  // namespace

JsonValue JsonParser::Parse(const std::string& text) {
    Cursor cursor(text);
    return cursor.ParseValue();
}
