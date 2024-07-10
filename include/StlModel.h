#ifndef STLMODEL_H
#define STLMODEL_H

/*!
 *  \file StlModel.h
 *  \brief Manipulation of STL model objects
 *  \author Julien LIVET
 *  \version 1.2
 *  \date 21/02/2018
 */

#include <array>
#include <string>
#include <vector>
#include <iostream>

#include <Eigen/Geometry>

class StlModel
{
    public:
        /*!
         *  \brief Facet, with its normal and its vertices
         */
        struct Facet
        {
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW

            Eigen::Vector3f normal;
            std::array<Eigen::Vector3f, 3> vertices;
        };

        /*!
         *  \brief Name of the model, used for ascii imports and exports
         */
        std::string name;

        /*!
         *  \brief Header of the model, used for binary imports and exports
         */
        std::array<char, 80> header;

        /*!
         *  \brief Default constructor for an empty model
         */
        StlModel();

        /*!
         *  \brief Constructor for an empty model but with a given name and a given haeader
         *  \param name_: name of the model, used for ascii exports
         *  \param header: header of the model, used for binary exports
         */
        explicit StlModel(std::string const& name_,
                          std::array<char, 80> const& header_);

        /*!
         *  \brief Constructor to load model from a file name
         *  \param fileName: the name of the file to read
         *  \param binaryFormat: true if the file data are binary, or false for ascii
         */
        explicit StlModel(std::string const& fileName, bool binaryFormat);

        /*!
         *  \brief Save the model to a file
         *  \param fileName: the name of the file to save
         *  \param binaryFormat: true if the file data are binary, or false for ascii
         *  \return true if the model was successfully saved
         */
        bool save(std::string const& fileName, bool binaryFormat) const;

        /*!
         *  \return the facets of the model
         */
        std::vector<Facet> const& facets() const;

        /*!
         *  \brief Reserve data before adding a lot of facets
         *  \param capacity: capacity of data to reserve
         */
        void reserve(std::size_t capacity);

        /*!
         *  \brief Load a file to the model, using tolerances of Type to join facets
         *  \param fileName: the name of the file to read
         *  \param binaryFormat: true if the file data are binary, or false for ascii
         *  \return true if the file was successfully read
         */
        bool load(std::string const& fileName, bool binaryFormat);

        /*!
         *  \brief Add a facet to the model, using tolerances of Type to join facets
         *  \param facet: facet to add
         */
        void addFacet(Facet const& facet);

        /*!
         *  \brief Add facets to the model
         *  \tparam Iterator: iterator of a container
         *  \param first: iterator of the first facet to add
         *  \param last: iterator after the last facet to add
         */
        template <typename Iterator>
        void addFacets(Iterator first, Iterator last);

    private:
        std::vector<Facet> facets_;
};

/*!
 *  \brief Output stl model in a stream
 *  \tparam _CharT: char template of basic_ostream
 *  \tparam _Traits: traits template of basic_ostream
 *  \param os: given output stream
 *  \param model: given model
 *  \return the output stream
 */
template<typename _CharT, typename _Traits>
std::basic_ostream<_CharT, _Traits>& operator<<(std::basic_ostream<_CharT, _Traits>& os,
                                                StlModel const& model);

#include "StlModel.tcc"

#endif // STLMODEL_H
