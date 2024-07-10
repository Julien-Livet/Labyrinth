#ifndef UTILITY_H
#define UTILITY_H

/*!
 *  \file Utility.h
 *  \brief Utility functions and objects
 *  \author Julien LIVET
 *  \version 1.2
 *  \date 26/02/2018
 */

#include <cmath>
#include <memory>
#include <queue>
#include <stdexcept>

#include <Eigen/Geometry>

namespace Utility
{
    /*!
     *  \brief Load a value (plain old types) from raw data.
     *  \param data: raw data queue
     *  \param value: loaded value
     *  \return the raw data for loading
     */
    template<typename T, typename S = T>
    std::queue<char>& load(std::queue<char>& data, T& value);

    /*!
     *  \brief Save a value (plain old types) to raw data.
     *  \param data: raw data queue
     *  \param value: saved value
     *  \return the raw data for saving
     */
    template<typename T, typename S = T>
    std::queue<char>& save(std::queue<char>& data, T const& value);

    /*!
     *  \brief Load an Eigen matrix base from raw data.
     *  \param data: raw data queue
     *  \param value: loaded value
     *  \return the raw data for loading
     */
    template <typename Derived, typename S = typename Derived::Scalar>
    std::queue<char>& loadMatrixBase(std::queue<char>& data, Eigen::MatrixBase<Derived>& value);

    /*!
     *  \brief Save an Eigen matrix base to raw data.
     *  \param data: raw data queue
     *  \param value: saved value
     *  \return the raw data for saving
     */
    template <typename T, typename S = typename T::Scalar>
    std::queue<char>& saveMatrixBase(std::queue<char>& data, Eigen::MatrixBase<T> const& value);

    /*!
     *  \brief Load a std::string from raw data.
     *  \param data: raw data queue
     *  \param value: loaded value
     *  \return the raw data for loading
     */
    std::queue<char>& loadString(std::queue<char>& data, std::string& value);

    /*!
     *  \brief Save a std::string to raw data.
     *  \param data: raw data queue
     *  \param value: saved value
     *  \return the raw data for saving
     */
    std::queue<char>& saveString(std::queue<char>& data, std::string const& value);

    /*!
     *  \brief Subtented angle between two 3D vectors.
     *  \param u: first 3D vector
     *  \param v: second 3D vector
     *  \return the subtented angle
     */
    template <typename Derived, typename OtherDerived>
    typename Derived::Scalar subtentedAngle(Eigen::MatrixBase<Derived> const& u,
                                            Eigen::MatrixBase<OtherDerived> const& v);

    /*!
     *  \brief An arbitrary normal vector of a 3D vector.
     *  \param u: 3D vector
     *  \param absoluteError: absolute error to consider the vector as null
     *  \return an arbitrary normal vector
     */
    template <typename Derived>
    Eigen::Matrix<typename Derived::Scalar, 3, 1> normal(Eigen::MatrixBase<Derived> const& u,
                                                         typename Derived::Scalar absoluteError = std::numeric_limits<typename Derived::Scalar>::epsilon());

    /*!
     *  \brief An arbitrary rotation matrix of a 3D vector.
     *  \param u: 3D vector, the third column of the computed matrix
     *  \param absoluteError: absolute error to consider the vector as null
     *  \return an arbitrary rotation matrix
     */
    template <typename Derived>
    Eigen::Matrix<typename Derived::Scalar, 3, 3> matrix(Eigen::MatrixBase<Derived> const& u,
                                                         typename Derived::Scalar absoluteError = std::numeric_limits<typename Derived::Scalar>::epsilon());

    template<typename T, typename = void>
    struct is_iterator
    {
       static constexpr bool value = false;
    };

    template<typename T>
    struct is_iterator<T, typename std::enable_if<!std::is_same<typename std::iterator_traits<T>::value_type, void>::value>::type>
    {
       static constexpr bool value = true;
    };

    template <typename Scalar>
    struct Number
    {
        static constexpr Scalar absoluteNormError()
        {
            return Scalar(1.0e-6);
        }

        static constexpr Scalar absoluteDistanceError()
        {
            return Scalar(1.0e-3);
        }

        static constexpr Scalar absoluteAngleError()
        {
            return Scalar(1.0e-3);
        }

        static constexpr Scalar relativeError()
        {
            return Scalar(1.0e-3);
        }

        static constexpr Scalar pi()
        {
            return Scalar(std::atan(Scalar{1.0}) * Scalar{4.0});
        }

        static constexpr char compare(Scalar x, Scalar y, Scalar absoluteError, Scalar relativeError)
        {
            if (x == y)
                return 0;

            auto const difference(std::abs(x - y));

            x = std::abs(x);
            y = std::abs(y);

            if (difference < absoluteError)
                return 0;

            if (x > y)
            {
                if (difference / x < relativeError)
                    return 0;
                else
                    return 1;
            }

            if (difference / y < relativeError)
                return 0;

            return -1;
        }

        static constexpr char compareDistance(Scalar x, Scalar y)
        {
            return compare(x, y, absoluteDistanceError(), relativeError());
        }

		static constexpr Scalar radians(Scalar deg)
		{
			return deg * pi() / Scalar{180.0};
		}

		static constexpr Scalar degrees(Scalar rad)
		{
			return rad * Scalar{180.0} / pi();
		}
    };

    typedef Number<float> Float;
    typedef Number<double> Double;

    template <typename Scalar>
    Eigen::Transform<Scalar, 3, Eigen::Affine> interpolate(Scalar t,
                                                           Eigen::Transform<Scalar, 3, Eigen::Affine> const& begin,
                                                           Eigen::Transform<Scalar, 3, Eigen::Affine> const& end);
}

#include "Utility.tcc"

#endif // UTILITY_H
