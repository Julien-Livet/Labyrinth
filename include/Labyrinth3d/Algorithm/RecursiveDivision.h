#ifndef LABYRINTH3D_ALGORITHM_RECURSIVEDIVISION_H
#define LABYRINTH3D_ALGORITHM_RECURSIVEDIVISION_H

/*!
 *  \file RecursiveDivision.h
 *  \brief Generate a perfect labyrinth by sub-divinding the grid
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <chrono>
#include <functional>
#include <tuple>
#include <vector>

#include "../Grid.h"
#include "../Labyrinth.h"

namespace Labyrinth3d
{
    namespace Algorithm
    {
        /*!
         *  \brief Generate a perfect labyrinth by sub-divinding the grid
         *
         *  The grid is divided randomly to reach 5 x 5 sub-grids.
         *  In each divsion, three of the four walls are randomly opened
         *  and apply the same algorithm for four sub-grids.
         */
        struct RecursiveDivision
        {
            /*!
             *  \brief Generate the labyrinth
             *
             *  \param g: uniform random number generator
             *  \param subGrid: sub-grid of labyrinth grid
             *  \param sleep: sleep function
             *  \param operationsCycle: number of operations in each cycle
             *  \param cyclePause: pause time between each cycle
             *  \param timeout: time before to abort generation
             */
            template <class URNG>
            void operator()(URNG& g, SubGrid<bool> const& subGrid,
                            std::function<void(std::chrono::milliseconds)> const& sleep = [] (std::chrono::milliseconds const&) -> void {},
                            size_t operationsCycle = 0,
                            std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0),
                            std::chrono::milliseconds const* timeout = nullptr);
        };
    }
}

template <class URNG>
void Labyrinth3d::Algorithm::RecursiveDivision::operator()(URNG& g, SubGrid<bool> const& subGrid,
                                                           std::function<void(std::chrono::milliseconds)> const& sleep,
                                                           size_t operationsCycle,
                                                           std::chrono::milliseconds const& cyclePause,
                                                           std::chrono::milliseconds const* timeout)
{
    auto const time(std::chrono::steady_clock::now());

    std::vector<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t> > history{std::make_tuple(0, 0, 0, subGrid.height() - 1, subGrid.width() - 1, subGrid.depth() - 1)};

    size_t operations(0);

    while (!history.empty())
    {
        size_t const l(history.size() - 1);//g() % history.size());

        auto const t(history[l]);
        size_t const h(std::get<3>(t) - std::get<0>(t) + 1);
        size_t const w(std::get<4>(t) - std::get<1>(t) + 1);
        size_t const d(std::get<5>(t) - std::get<2>(t) + 1);
        size_t const i(h > 3 ? (g() % (std::get<3>(t) - std::get<0>(t) - 2) / 2 + 1) * 2 + std::get<0>(t) : std::get<0>(t));
        size_t const j(w > 3 ? (g() % (std::get<4>(t) - std::get<1>(t) - 2) / 2 + 1) * 2 + std::get<1>(t) : std::get<1>(t));
        size_t const k(d > 3 ? (g() % (std::get<5>(t) - std::get<2>(t) - 2) / 2 + 1) * 2 + std::get<2>(t) : std::get<2>(t));

        history[l] = history.back();
        history.pop_back();

        if (h > 3 && w > 3 && d > 3)
        {
            if (subGrid.grid().labyrinth().state() & Labyrinth::StopGenerating)
                return;

            if (operationsCycle && cyclePause.count() && !(operations % operationsCycle))
                sleep(cyclePause);

            if (timeout != nullptr)
            {
                if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time) > *timeout)
                    throw TimeoutException();
            }

            for (size_t iTmp(std::get<0>(t) + 1); iTmp < std::get<3>(t); iTmp += 2)
                subGrid.set(iTmp, j, k);

            for (size_t jTmp(std::get<1>(t) + 1); jTmp < std::get<4>(t); jTmp += 2)
                subGrid.set(i, jTmp, k);

            for (size_t kTmp(std::get<2>(t) + 1); kTmp < std::get<5>(t); kTmp += 2)
                subGrid.set(i, j, kTmp);

            std::vector<Direction> d{Up, Right, Down, Left, Back, Front};

            while (d.size() > 1)
            {
                size_t v(g() % d.size());

                switch (d[v])
                {
                    case Up:
                        if (i - std::get<0>(t) - 2)
                            subGrid.toggle(i - (g() % ((i - std::get<0>(t) - 2) / 2)) * 2 - 1, j, k);
                        else
                            subGrid.toggle(i - 1, j, k);
                        break;

                    case Right:
                        if (std::get<4>(t) - j - 2)
                            subGrid.toggle(i, j + (g() % ((std::get<4>(t) - j - 2) / 2)) * 2 + 1, k);
                        else
                            subGrid.toggle(i, j + 1, k);
                        break;

                    case Down:
                        if (std::get<3>(t) - i - 2)
                            subGrid.toggle(i + (g() % ((std::get<3>(t) - i - 2) / 2)) * 2 + 1, j, k);
                        else
                            subGrid.toggle(i + 1, j, k);
                        break;

                    case Left:
                        if (j - std::get<1>(t) - 2)
                            subGrid.toggle(i, j - (g() % ((j - std::get<1>(t) - 2) / 2)) * 2 - 1, k);
                        else
                            subGrid.toggle(i, j - 1, k);
                        break;

                    case Back:
                        if (k - std::get<2>(t) - 2)
                            subGrid.toggle(i, j, k - (g() % ((k - std::get<2>(t) - 2) / 2)) * 2 - 1);
                        else
                            subGrid.toggle(i, j, k - 1);
                        break;

                    case Front:
                        if (std::get<5>(t) - k - 2)
                            subGrid.toggle(i, j, k + (g() % ((std::get<5>(t) - k - 2) / 2)) * 2 + 1);
                        else
                            subGrid.toggle(i, j, k + 1);
                        break;
                }

                d[v] = d.back();
                d.pop_back();
            }

            ++operations;
        }

        size_t hTmp(i - std::get<0>(t) + 1);
        size_t wTmp(j - std::get<1>(t) + 1);
        size_t dTmp(k - std::get<2>(t) + 1);

        if (hTmp != 1 && wTmp != 1 && dTmp != 1 && (hTmp > 3 || wTmp > 3 || dTmp > 3))
            history.push_back(std::make_tuple(std::get<0>(t), std::get<1>(t), std::get<2>(t), i, j, k));

        hTmp = i - std::get<0>(t) + 1;
        wTmp = std::get<4>(t) - j + 1;

        if (hTmp != 1 && wTmp != 1 && dTmp != 1 && (hTmp > 3 || wTmp > 3 || dTmp > 3))
            history.push_back(std::make_tuple(std::get<0>(t), j, std::get<2>(t), i, std::get<4>(t), k));

        hTmp = std::get<3>(t) - i + 1;
        wTmp = std::get<4>(t) - j + 1;

        if (hTmp != 1 && wTmp != 1 && dTmp != 1 && (hTmp > 3 || wTmp > 3 || dTmp > 3))
            history.push_back(std::make_tuple(i, j, std::get<2>(t), std::get<3>(t), std::get<4>(t), k));

        hTmp = std::get<3>(t) - i + 1;
        wTmp = j - std::get<1>(t) + 1;

        if (hTmp != 1 && wTmp != 1 && dTmp != 1 && (hTmp > 3 || wTmp > 3 || dTmp > 3))
            history.push_back(std::make_tuple(i, std::get<1>(t), std::get<5>(t), std::get<3>(t), j, k));

        dTmp = std::get<5>(t) - k + 1;
        hTmp = i - std::get<0>(t) + 1;
        wTmp = j - std::get<1>(t) + 1;

        if (hTmp != 1 && wTmp != 1 && dTmp != 1 && (hTmp > 3 || wTmp > 3 || dTmp > 3))
            history.push_back(std::make_tuple(std::get<0>(t), std::get<1>(t), k, i, j, std::get<5>(t)));

        hTmp = i - std::get<0>(t) + 1;
        wTmp = std::get<4>(t) - j + 1;

        if (hTmp != 1 && wTmp != 1 && dTmp != 1 && (hTmp > 3 || wTmp > 3 || dTmp > 3))
            history.push_back(std::make_tuple(std::get<0>(t), j, k, i, std::get<4>(t), std::get<5>(t)));

        hTmp = std::get<3>(t) - i + 1;
        wTmp = std::get<4>(t) - j + 1;

        if (hTmp != 1 && wTmp != 1 && dTmp != 1 && (hTmp > 3 || wTmp > 3 || dTmp > 3))
            history.push_back(std::make_tuple(i, j, k, std::get<3>(t), std::get<4>(t), std::get<5>(t)));

        hTmp = std::get<3>(t) - i + 1;
        wTmp = j - std::get<1>(t) + 1;

        if (hTmp != 1 && wTmp != 1 && dTmp != 1 && (hTmp > 3 || wTmp > 3 || dTmp > 3))
            history.push_back(std::make_tuple(i, std::get<1>(t), k, std::get<3>(t), j, std::get<5>(t)));
    }
}

#endif // LABYRINTH3D_ALGORITHM_RECURSIVEDIVISION_H
