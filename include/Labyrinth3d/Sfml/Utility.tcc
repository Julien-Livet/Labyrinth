template<typename T, typename S>
std::queue<char>& Utility::load(std::queue<char>& data, T& value)
{
    if (!std::is_same<S, T>::value)
    {
        S v{};

        load<S, S>(data, v);

        value = static_cast<T>(v);

        return data;
    }

    if (data.size() < sizeof(T))
        throw std::runtime_error("Invalid data size for loading!");

    std::vector<char> buffer{};

    buffer.reserve(sizeof(T));

    for (std::size_t i(0); i < sizeof(T); ++i)
    {
        buffer.push_back(data.front());
        data.pop();
    }

    std::memcpy(&value, buffer.data(), sizeof(T));

    return data;
}

template<typename T, typename S>
std::queue<char>& Utility::save(std::queue<char>& data, T const& value)
{
    if (!std::is_same<S, T>::value)
        return save<S, S>(data, static_cast<S>(value));

    std::vector<char> buffer(sizeof(T));

    std::memcpy(buffer.data(), &value, sizeof(T));

    for (auto const& c : buffer)
        data.push(c);

    return data;
}

template <typename Derived, typename S>
std::queue<char>& Utility::loadMatrixBase(std::queue<char>& data, Eigen::MatrixBase<Derived>& value)
{
    for (decltype(value.rows()) i(0); i < value.rows(); ++i)
        for (decltype(value.cols()) j(0); j < value.cols(); ++j)
            load<typename Derived::Scalar, S>(data, value(i, j));

    return data;
}

template <typename Derived, typename S>
std::queue<char>& Utility::saveMatrixBase(std::queue<char>& data, Eigen::MatrixBase<Derived> const& value)
{
    for (decltype(value.rows()) i(0); i < value.rows(); ++i)
        for (decltype(value.cols()) j(0); j < value.cols(); ++j)
            save<typename Derived::Scalar, S>(data, value(i, j));

    return data;
}

template <typename Derived, typename OtherDerived>
typename Derived::Scalar Utility::subtentedAngle(Eigen::MatrixBase<Derived> const& u,
                                                        Eigen::MatrixBase<OtherDerived> const& v)
{
    return std::atan2(u.cross(v).norm(), u.dot(v));
}

template <typename Derived>
Eigen::Matrix<typename Derived::Scalar, 3, 1> Utility::normal(Eigen::MatrixBase<Derived> const& u,
                                                                     typename Derived::Scalar absoluteError)
{
    assert(absoluteError >= 0.0);

    if (u.norm() < absoluteError)
        throw std::runtime_error("Null norm!");

    Eigen::Matrix<typename Derived::Scalar, 3, 1> const v(u.cross(Eigen::MatrixBase<Derived>::UnitX()));

    if (v.norm() >= absoluteError)
        return v;

    Eigen::Matrix<typename Derived::Scalar, 3, 1> const w(u.cross(Eigen::MatrixBase<Derived>::UnitY()));

    if (w.norm() >= absoluteError)
        return w;

    return u.cross(Eigen::MatrixBase<Derived>::UnitZ());
}

template <typename Derived>
Eigen::Matrix<typename Derived::Scalar, 3, 3> Utility::matrix(Eigen::MatrixBase<Derived> const& u,
                                                                     typename Derived::Scalar absoluteError)
{
    assert(absoluteError >= 0.0);

    if (u.norm() < absoluteError)
        throw std::runtime_error("Null norm!");

    auto const n(normal(u));

    return (Eigen::Matrix<typename Derived::Scalar, 3, 3>{} << n.cross(u).normalized(),
                                                               n.normalized(),
                                                               u.normalized()).finished();
}

template <typename Scalar>
Eigen::Transform<Scalar, 3, Eigen::Affine> Utility::interpolate(Scalar t,
                                                                Eigen::Transform<Scalar, 3, Eigen::Affine> const& begin,
                                                                Eigen::Transform<Scalar, 3, Eigen::Affine> const& end)
{
    Eigen::Transform<Scalar, 3, Eigen::Affine> transform{begin};

    transform.linear() = Eigen::Quaternionf{begin.linear()}.slerp(t, Eigen::Quaternionf{end.linear()}).toRotationMatrix();
    transform.translation() = (1.0 - t) * begin.translation() + t * end.translation();

    return transform;
}
