#include "Labyrinth3d/Sfml/Utility.h"

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

glm::mat4 Utility::eigenToGlm(Eigen::Transform<float, 3, Eigen::Affine> const& t)
{
    glm::mat4 m{1.0};

    for (unsigned char i{0}; i < 3; ++i)
        for (unsigned char j{0}; j < 4; ++j)
            m[j][i] = t(i, j);

    return m;
}

glm::mat4 Utility::eigenToGlm(Eigen::Transform<float, 3, Eigen::Projective> const& t)
{
    glm::mat4 m{0.0};

    for (unsigned char i{0}; i < 4; ++i)
        for (unsigned char j{0}; j < 4; ++j)
            m[j][i] = t(i, j);

    return m;
}

glm::mat4 Utility::eigenToGlm(Eigen::Matrix4f const& t)
{
    glm::mat4 m{0.0};

    for (unsigned char i{0}; i < 4; ++i)
        for (unsigned char j{0}; j < 4; ++j)
            m[j][i] = t(i, j);

    return m;
}

Eigen::Matrix4f Utility::glmToEigen(glm::mat4 const& m)
{
    Eigen::Matrix4f t{Eigen::Matrix4f::Zero()};

    for (unsigned char i{0}; i < 4; ++i)
        for (unsigned char j{0}; j < 4; ++j)
            t(i, j) = m[j][i];

    return t;
}

glm::mat4 Utility::glmFromEigen(Eigen::Matrix4f const& m)
{
    return eigenToGlm(m);
}

Eigen::Matrix4f Utility::eigenFromGlm(glm::mat4 const& m)
{
    return glmToEigen(m);
}

glm::vec3 Utility::eigenToGlm3(Eigen::Vector3f const& v)
{
    return glm::vec3{v[0], v[1], v[2]};
}

Eigen::Vector3f Utility::glmToEigen(glm::vec3 const& v)
{
    return (Eigen::Vector3f{} << v[0], v[1], v[2]).finished();
}

glm::vec3 Utility::glmFromEigen3(Eigen::Vector3f const& v)
{
    return eigenToGlm3(v);
}

Eigen::Vector3f Utility::eigenFromGlm(glm::vec3 const& v)
{
    return glmToEigen(v);
}
