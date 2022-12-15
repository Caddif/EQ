/*
  ==============================================================================

    Params.h
    Created: 2 Nov 2022 6:59:54pm
    Author:  Filip Tomeczek

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
    float HiPassFreq{ 0 }, lowPassFreq{ 0 };
    float peakFreq{ 0 }, peakGainInDb{ 0 }, peakQuality{ 1.f };
    float peakFreq2{ 0 }, peakGainInDb2{ 0 }, peakQuality2{ 1.f };
    float peakFreq3{ 0 }, peakGainInDb3{ 0 }, peakQuality3{ 1.f };
    Slope HiPassSlope{ Slope::Slope12 }, lowPassSlope{ Slope::Slope12 };
    float totalGain{ 0 };
};

enum Chainpositons
{
    HiPass,
    Peak,
    Peak2,
    Peak3,
    LowPass,
    Gain
};

template<int Id, typename Chaintype, typename CoefficientType>
void update(Chaintype& chain, const CoefficientType& coefficeints)
{
    updateCoeffs(chain.template get<Id>().coefficients, coefficeints[Id]);
    chain.template setBypassed<Id>(false);
}

template<typename ChainType, typename CoefficientType>
void updatePassFilter(ChainType& chain, const CoefficientType& cutCoeffs, const Slope& passSlope)
{
    chain.template setBypassed<0>(true);
    chain.template setBypassed<1>(true);
    chain.template setBypassed<2>(true);
    chain.template setBypassed<3>(true);

    switch (passSlope)
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