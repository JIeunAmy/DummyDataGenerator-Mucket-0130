#pragma once

#include <map>
#include <string>
#include <vector>

// data/sample.json, data/order.json 같은 단순 JSON 문서를 읽기 위한
// 최소 구현 파서 (외부 의존성 없이 표준 라이브러리만 사용).
enum class JsonType { Null, Bool, Number, String, Array, Object };

class JsonValue {
public:
    JsonType type = JsonType::Null;
    bool boolValue = false;
    double numberValue = 0.0;
    std::string stringValue;
    std::vector<JsonValue> arrayValue;
    std::map<std::string, JsonValue> objectValue;

    double AsDouble() const;
    int AsInt() const;
    const std::string& AsString() const;
    const std::vector<JsonValue>& AsArray() const;

    // 객체에서 key를 찾지 못하면 Null JsonValue를 반환한다.
    const JsonValue& Get(const std::string& key) const;
};

class JsonParser {
public:
    static JsonValue Parse(const std::string& text);
};
