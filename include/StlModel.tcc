#include <iterator>

template <typename Iterator>
void StlModel::addFacets(Iterator first, Iterator last)
{
    reserve(facets_.size() + std::distance(first, last));

    for (; first != last; ++first)
        addFacet(*first);
}

template<typename _CharT, typename _Traits>
std::basic_ostream<_CharT, _Traits>& operator<<(std::basic_ostream<_CharT, _Traits>& os,
                                                StlModel const& model)
{
    os << "> Name: " << model.name << '\n';

    os << "> Facets number: " << model.facets().size() << '\n';

    for (std::size_t i{0}; i < model.facets().size(); ++i)
    {
        os << "Facet #" << i << ":" << '\n';

        os << "n" << 1 + i << ": " << model.facets()[i].normal.transpose() << '\n';

        for (unsigned char j{0}; j < 3; ++j)
            os << "v" << 1 + i << "_" << 1 + j << ": " << model.facets()[i].vertices[j].transpose() << '\n';
    }

    return os;
}
