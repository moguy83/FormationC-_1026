#pragma once 

#include <stdexcept>

inline int calculerPrixRemise(int prix, bool fidele)
{
    if (prix < 0)
    {
        throw std::invalid_argument("Prix negatif");
    }

    if (prix == 0)
    {
        return 0;
    }
    if (prix >= 100)
    {
        return prix - 20;
    }
    if (fidele)
    {
        return prix - 5;
    }

    return prix;
}