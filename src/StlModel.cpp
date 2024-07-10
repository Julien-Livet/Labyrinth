#include <map>
#include <algorithm>
#include <fstream>
#include <cassert>

#include "StlModel.h"
#include "Utility.h"

StlModel::StlModel()
    : name{}, header{}, facets_{}
{
    header.fill(0);
}

StlModel::StlModel(std::string const& name_, std::array<char, 80> const& header_)
    : name(name_), header(header_), facets_{}
{
}

StlModel::StlModel(std::string const& fileName, bool binaryFormat)
{
    load(fileName, binaryFormat);
}

bool StlModel::save(std::string const& fileName, bool binaryFormat) const
{
    if (binaryFormat)
    {
        std::ofstream output{fileName, std::ios_base::out
                                       | std::ios_base::trunc
                                       | std::ios_base::binary};

        if (!output)
            return false;

        std::queue<char> data{};

        for (auto const& c : header)
            data.push(c);

        Utility::save(data, static_cast<uint32_t>(facets_.size()));

        for (auto&& f : facets_)
        {
            Utility::saveMatrixBase(data, f.normal);

            for (auto&& v : f.vertices)
                Utility::saveMatrixBase(data, v);

            Utility::save(data, static_cast<uint16_t>(0));
        }

        while (!data.empty())
        {
            output.put(data.front());
            data.pop();
        }
    }
    else
    {
        std::ofstream output{fileName, std::ios_base::out
                                       | std::ios_base::trunc};

        if (!output)
            return false;

        ///output.setf(std::ios::scientific);

        output << "solid " << name << '\n';

        for (auto&& f : facets_)
        {
        output << "  facet normal " << f.normal.transpose().format(Eigen::IOFormat{Eigen::FullPrecision, Eigen::DontAlignCols}) << '\n';
            output << "    outer loop\n";

            for (auto&& v : f.vertices)
                output << "      vertex " << v.transpose().format(Eigen::IOFormat{Eigen::FullPrecision, Eigen::DontAlignCols}) << '\n';

            output << "    endloop\n";
            output << "  endfacet\n";
        }

        output << "endsolid " << name << '\n';
    }

    return true;
}

void StlModel::reserve(std::size_t capacity)
{
    facets_.reserve(capacity);
}

std::vector<StlModel::Facet> const& StlModel::facets() const
{
    return facets_;
}

bool StlModel::load(std::string const& fileName, bool binaryFormat)
{
    name.clear();
    header.fill(0);
    facets_.clear();

    if (binaryFormat)
    {
        std::ifstream input{fileName, std::ios_base::in
                                      | std::ios_base::binary};

        if (!input)
            return false;

        std::queue<char> data{};

        while (!input.eof())
        {
            char c{0};

            input.get(c);

            if (!input.eof())
                data.push(c);
        }

        for (auto& c : header)
        {
            c = data.front();
            data.pop();
        }

        uint32_t n{0};

        Utility::load(data, n);

        reserve(n);

        for (; n; --n)
        {
            Facet f{};

            Utility::loadMatrixBase(data, f.normal);

            for (auto& v : f.vertices)
                Utility::loadMatrixBase(data, v);

            uint16_t byteCount{0};

            Utility::load(data, byteCount);

            addFacet(f);
        }
    }
    else
    {
        std::ifstream input{fileName, std::ios_base::in};

        if (!input)
            return false;

        ///input.setf(std::ios::scientific);

        std::string s{};

        std::runtime_error const eofException{"End of file unexpected!"};

        auto readString = [&input, &s, &eofException]
                          (std::string const& word)
                          -> void
                          {
                              if (input.eof())
                                  throw eofException;

                              input >> s;

                              if (s != word)
                                  throw std::runtime_error{"'" + word + "' word expected!"};
                          };

        auto readFloat = [&input, &eofException]
                         (float& f)
                         -> void
                         {
                             if (input.eof())
                                 throw eofException;

                              input >> f;
                         };

        readString("solid");

        {
             if (input.eof())
                 throw eofException;

            std::getline(input, name);

            if (name.empty())
                throw eofException;

            name.erase(name.begin());
        }

        while (true)
        {
            Facet f{};

            try
            {
                readString("facet");
            }
            catch (std::runtime_error const& e)
            {
                if (s == "end")
                {
                    if (input.eof())
                        throw eofException;

                    input >> s;

                    if (s != "solid")
                        throw std::runtime_error{"'solid' word expected!"};
                }
                else if (s != "endsolid")
                    throw std::runtime_error{"'endsolid' word expected!"};

                if (input.eof())
                    throw eofException;

                std::getline(input, s);

                if (!s.empty())
                {
                    s.erase(s.begin());

                    if (s != name)
                        throw std::runtime_error{"'" + name + "' word expected!"};
                }

                break;
            }

            readString("normal");

            readFloat(f.normal[0]);
            readFloat(f.normal[1]);
            readFloat(f.normal[2]);

            readString("outer");
            readString("loop");

            for (auto& v : f.vertices)
            {
                readString("vertex");

                readFloat(v[0]);
                readFloat(v[1]);
                readFloat(v[2]);
            }

            readString("endloop");
            readString("endfacet");

            addFacet(f);
        }
    }

    return true;
}

void StlModel::addFacet(Facet const& facet)
{
    facets_.push_back(facet);
}
