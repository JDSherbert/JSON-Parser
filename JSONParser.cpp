// ©2023 JDSherbert. All rights reserved.

#include <iostream>
#include <string>
#include <vector>
#include <map>

// Custom structure to hold parsed JSON data
struct JsonData 
{
    std::map<std::string, std::string> properties;
    std::map<std::string, JsonData> objects;
    std::map<std::string, std::vector<JsonData>> arrays;
};

// Helper function to trim leading and trailing whitespace from a string
std::string trim(const std::string& str) 
{
    size_t first = str.find_first_not_of(" \t\n");
    size_t last = str.find_last_not_of(" \t\n");
    if (first == std::string::npos || last == std::string::npos) {
        return "";
    }
    return str.substr(first, last - first + 1);
}

// JSON parsing function
JsonData parse_json(const std::string& json) 
{
    JsonData result;

    std::string trimmed_json = trim(json);

    if (trimmed_json.empty() || trimmed_json[0] != '{' || trimmed_json[trimmed_json.length() - 1] != '}') 
    {
        std::cout << "Invalid JSON format." << std::endl;
        return result;
    }

    std::string json_body = trimmed_json.substr(1, trimmed_json.length() - 2);

    // Iterate over key-value pairs in JSON
    size_t pos = 0;
    while (pos < json_body.length()) 
    {
        // Find key
        size_t key_start = json_body.find_first_not_of(" \t\n", pos);
        size_t key_end = json_body.find(':', key_start);
        if (key_start == std::string::npos || key_end == std::string::npos) 
        {
            std::cout << "Invalid JSON format." << std::endl;
            return result;
        }

        std::string key = trim(json_body.substr(key_start, key_end - key_start));

        // Find value
        size_t value_start = json_body.find_first_not_of(" \t\n", key_end + 1);
        if (value_start == std::string::npos) 
        {
            std::cout << "Invalid JSON format." << std::endl;
            return result;
        }

        char value_start_char = json_body[value_start];
        if (value_start_char == '{') 
        {
            // Object value
            size_t object_end = json_body.find('}', value_start);
            if (object_end == std::string::npos) 
            {
                std::cout << "Invalid JSON format." << std::endl;
                return result;
            }

            std::string object_value = json_body.substr(value_start, object_end - value_start + 1);
            result.objects[key] = parse_json(object_value);

            pos = object_end + 1;
        }
        else if (value_start_char == '[') 
        {
            // Array value
            size_t array_end = json_body.find(']', value_start);
            if (array_end == std::string::npos) 
            {
                std::cout << "Invalid JSON format." << std::endl;
                return result;
            }

            std::string array_value = json_body.substr(value_start, array_end - value_start + 1);
            std::vector<JsonData> array_data;

            // Parse array elements recursively
            size_t array_pos = 1;
            while (array_pos < array_value.length() - 1) 
            {
                size_t next_comma = array_value.find(',', array_pos);
                size_t element_end = (next_comma != std::string::npos) ? next_comma : array_value.length() - 1;

                std::string array_element = trim(array_value.substr(array_pos, element_end - array_pos));
                if (!array_element.empty()) 
                {
                    array_data.push_back(parse_json(array_element));
                }

                array_pos = element_end + 1;
            }

            result.arrays[key] = array_data;
            pos = array_end + 1;
        }
        else 
        {
            // String value
            size_t value_end;
            if (value_start_char == '"' || value_start_char == '\'') 
            {
                value_end = json_body.find(value_start_char, value_start + 1);
                if (value_end == std::string::npos) 
                {
                    std::cout << "Invalid JSON format." << std::endl;
                    return result;
                }
            }
            else 
            {
                value_end = json_body.find_first_of(",}", value_start);
                if (value_end == std::string::npos) 
                {
                    std::cout << "Invalid JSON format." << std::endl;
                    return result;
                }
            }

            std::string value = trim(json_body.substr(value_start, value_end - value_start));
            result.properties[key] = value;

            pos = value_end + 1;
        }
    }

    return result;
}
