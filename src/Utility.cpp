#include "Utility.h"

std::queue<char>& Utility::loadString(std::queue<char>& data, std::string& value)
{
    value.clear();

    bool ok(true);

    while (ok)
    {
        if (data.empty())
            throw std::runtime_error("Invalid data size for loading!");

        if (!data.front())
            ok = false;
        else
            value.push_back(data.front());

        data.pop();
    }

    return data;
}

std::queue<char>& Utility::saveString(std::queue<char>& data, std::string const& value)
{
    for (auto const& c : value)
        data.push(c);

    data.push(0);

    return data;
}
