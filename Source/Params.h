/*
  ==============================================================================

    Params.h
    Created: 16 Nov 2022 6:59:54pm
    Author:  user

  ==============================================================================
*/

#pragma once

enum Slope
{
    Slope12,
    Slope24,
    Slope36,
    Slope48
};

struct ChainSettings {
    float peakFreq{ 0 }, peakGainInDb{ 0 }, peakQuality{ 1.f };
    float lowCutFreq{ 0 }, hiCutFreq{ 0 };
    Slope lowCutSlope{ Slope::Slope12 }, hiCutSlope{ Slope::Slope12 };
};

enum Chainpositons
{
    LowCut,
    Peak,
    HiCut
};

template<int Id, typename Chaintype, typename CoefficientType>
void update(Chaintype& chain, const CoefficientType& coefficeints)
{
    updateCoeffs(chain.template get<Id>().coefficients, coefficeints[Id]);
    chain.template setBypassed<Id>(false);
}

template<typename ChainType, typename CoefficientType>
void updateCutFilter(ChainType& chain, const CoefficientType& cutCoeffs, const Slope& lowCutSlope)
{
    chain.template setBypassed<0>(true);
    chain.template setBypassed<1>(true);
    chain.template setBypassed<2>(true);
    chain.template setBypassed<3>(true);

    switch (lowCutSlope)
    {
    case Slope48:
    {
        update<3>(chain, cutCoeffs);
    }
    case Slope36:
    {
        update<2>(chain, cutCoeffs);
    }
    case Slope24:
    {
        update<1>(chain, cutCoeffs);
    }
    case Slope12:
    {
        update<0>(chain, cutCoeffs);
    }
    }

}